/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Damien P. George
 * Copyright (c) 2018 Alan Dragomirecky
 * Copyright (c) 2020 Antoine Aubert
 * Copyright (c) 2021, 2023-2025 Ihor Nehrutsa
 * Copyright (c) 2024 Yoann Darche
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// This file is never compiled standalone, it's included directly from
// extmod/machine_pwm.c via MICROPY_PY_MACHINE_PWM_INCLUDEFILE.

#include <math.h>
#include "py/mphal.h"
#include "esp_err.h"
#include "driver/ledc.h"
#include "soc/ledc_periph.h"
#include "soc/gpio_sig_map.h"
#include "esp_clk_tree.h"
#include "py/mpprint.h"

#define debug_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__); mp_printf(&mp_plat_print, " | %d at %s\n", __LINE__, __FILE__);

#define FADE 0

// 10-bit user interface resolution compatible with esp8266 PWM.duty()
#define UI_RES_10_BIT  (10)
#define DUTY_10        UI_RES_10_BIT
// Maximum duty value on 10-bit resolution is 1024 but reduced to 1023 in UI
#define MAX_10_DUTY    (1U << UI_RES_10_BIT)

// 16-bit user interface resolution used in PWM.duty_u16()
#define UI_RES_16_BIT  (16)
#define DUTY_16        UI_RES_16_BIT
// Maximum duty value on 16-bit resolution is 65536 but reduced to 65535 in UI
#define MAX_16_DUTY    (1U << UI_RES_16_BIT)

// ns user interface used in PWM.duty_ns()
#define DUTY_NS        (1)

// 5khz is default frequency
#define PWM_FREQ       (5000)
// default duty 50%
#define PWM_DUTY       ((1U << UI_RES_16_BIT) / 2)

// All chips except esp32 and esp32s2 do not have timer-specific clock sources, which means clock source for all timers must be the same one.
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
// If the PWM frequency is less than EMPIRIC_FREQ, then LEDC_REF_CLK_HZ(1 MHz) source is used, else LEDC_APB_CLK_HZ(80 MHz) source is used
#define EMPIRIC_FREQ   (10) // Hz
#endif

// Config of timer upon which we run all PWM'ed GPIO pins
static bool pwm_inited = false;

// MicroPython PWM object struct
typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    int8_t pin;
    int8_t mode;
    int8_t channel;
    int8_t timer;
    bool lightsleep;
    int32_t freq;
    int8_t duty_scale; // DUTY_10 if duty(), DUTY_16 if duty_u16(), DUTY_NS if duty_ns()
    int duty_ui;       // saved values of UI duty
    int channel_duty;  // saved values of UI duty, calculated to raw channel->duty
    bool output_invert;
    bool output_invert_prev;
} machine_pwm_obj_t;

typedef struct _chans_t {
    int8_t pin;      // Which channel has which GPIO pin assigned? (-1 if not assigned)
    int8_t timer;    // Which channel has which timer assigned? (-1 if not assigned)
    bool lightsleep; // Is light sleep enable has been set for this pin
} chans_t;

// List of PWM channels
static chans_t chans[LEDC_SPEED_MODE_MAX][LEDC_CHANNEL_MAX];

typedef struct _timers_t {
    int32_t freq;
    int8_t duty_resolution;
    ledc_clk_cfg_t clk_cfg;
} timers_t;

// List of PWM timers
static timers_t timers[LEDC_SPEED_MODE_MAX][LEDC_TIMER_MAX];

// register-unregister channel
static void register_channel(int mode, int channel, int pin, int timer) {
    chans[mode][channel].pin = pin;
    chans[mode][channel].timer = timer;
}

static void unregister_channel(int mode, int channel) {
    register_channel(mode, channel, -1, -1);
    chans[mode][channel].lightsleep = false;
}

static void unregister_timer(int mode, int timer) {
    timers[mode][timer].freq = -1; // unused timer freq is -1
    timers[mode][timer].duty_resolution = 0;
    timers[mode][timer].clk_cfg = LEDC_AUTO_CLK;
}

static void pwm_init(void) {
    for (int mode = 0; mode < LEDC_SPEED_MODE_MAX; ++mode) {
        // Initial condition: no channels assigned
        for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
            unregister_channel(mode, channel);
        }
        // Initial condition: no timers assigned
        for (int timer = 0; timer < LEDC_TIMER_MAX; ++timer) {
            unregister_timer(mode, timer);
        }
    }
}

// Returns true if the timer is in use in addition to current channel
static bool is_timer_in_use(int mode, int current_channel, int timer) {
    for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
        if ((channel != current_channel) && (chans[mode][channel].timer == timer) && (chans[mode][channel].pin >= 0)) {
            return true;
        }
    }
    return false;
}

// Deinit channel and timer if the timer is unused
static void pwm_deinit(int mode, int channel, int level) {
    // Is valid channel?
    if ((mode >= 0) && (mode < LEDC_SPEED_MODE_MAX) && (channel >= 0) && (channel < LEDC_CHANNEL_MAX)) {
        // Clean up timer if necessary
        int timer = chans[mode][channel].timer;
        if (timer >= 0) {
            if (!is_timer_in_use(mode, channel, timer)) {
                check_esp_err(ledc_timer_pause(mode, timer));
                ledc_timer_config_t ledc_timer = {
                    .deconfigure = true,
                    .speed_mode = mode,
                    .timer_num = timer,
                };
                ledc_timer_config(&ledc_timer);
                unregister_timer(mode, timer);
            }
        }

        int pin = chans[mode][channel].pin;
        if (pin >= 0) {
            // Disable LEDC output, and set idle level
            check_esp_err(ledc_stop(mode, channel, level));
            if (chans[mode][channel].lightsleep) {
                // Enable SLP_SEL to change GPIO status automantically in lightsleep.
                check_esp_err(gpio_sleep_sel_en(pin));
                chans[mode][channel].lightsleep = false;
            }
        }
        unregister_channel(mode, channel);
    }
}

// This called from Ctrl-D soft reboot
void machine_pwm_deinit_all(void) {
    if (pwm_inited) {
        for (int mode = 0; mode < LEDC_SPEED_MODE_MAX; ++mode) {
            for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
                pwm_deinit(mode, channel, 0);
            }
        }
        #if FADE
        ledc_fade_func_uninstall();
        #endif
        pwm_inited = false;
    }
}

static void pwm_is_active(machine_pwm_obj_t *self) {
    if (self->timer < 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("PWM is inactive"));
    }
}

// Calculate the duty parameters based on an ns value
static int ns_to_duty(machine_pwm_obj_t *self, int ns) {
    pwm_is_active(self);
    int64_t duty = ((int64_t)ns * (int64_t)MAX_16_DUTY * self->freq + 500000000LL) / 1000000000LL;
    if ((ns > 0) && (duty == 0)) {
        duty = 1;
    } else if (duty > MAX_16_DUTY) {
        duty = MAX_16_DUTY;
    }
    return duty;
}

static int duty_to_ns(machine_pwm_obj_t *self, int duty) {
    pwm_is_active(self);
    return ((int64_t)duty * 1000000000LL + (int64_t)self->freq * (int64_t)(MAX_16_DUTY / 2)) / ((int64_t)self->freq * (int64_t)MAX_16_DUTY);
}

// Reconfigure PWM pin output as input/output.
static void reconfigure_pin(machine_pwm_obj_t *self) {
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 4, 0)
    // This allows to read the pin level.
    gpio_set_direction(self->pin, GPIO_MODE_INPUT_OUTPUT);
    #endif
    esp_rom_gpio_connect_out_signal(self->pin, ledc_periph_signal[self->mode].sig_out0_idx + self->channel, self->output_invert, 0);
}

static void apply_duty(machine_pwm_obj_t *self) {
    pwm_is_active(self);

    int duty = 0;
    if (self->duty_scale == DUTY_16) {
        duty = self->duty_ui;
    } else if (self->duty_scale == DUTY_10) {
        duty = self->duty_ui << (UI_RES_16_BIT - UI_RES_10_BIT);
    } else if (self->duty_scale == DUTY_NS) {
        duty = ns_to_duty(self, self->duty_ui);
    }
    self->channel_duty = duty >> (UI_RES_16_BIT - timers[self->mode][self->timer].duty_resolution);
    if ((chans[self->mode][self->channel].pin == -1) || (self->output_invert_prev != self->output_invert)) {
        self->output_invert_prev = self->output_invert;
        // New PWM assignment
        ledc_channel_config_t cfg = {
            .channel = self->channel,
            .duty = self->channel_duty,
            .gpio_num = self->pin,
            .intr_type = LEDC_INTR_DISABLE,
            .speed_mode = self->mode,
            .timer_sel = self->timer,
            .hpoint = 0,
            .flags.output_invert = self->output_invert,
        };
        check_esp_err(ledc_channel_config(&cfg));
        reconfigure_pin(self);
    } else {
        #if FADE
        check_esp_err(ledc_set_duty_and_update(self->mode, self->channel, self->channel_duty, 0));
        #else
        check_esp_err(ledc_set_duty(self->mode, self->channel, self->channel_duty));
        check_esp_err(ledc_update_duty(self->mode, self->channel));
        #endif
    }
    if (self->lightsleep) {
        // Disable SLP_SEL to change GPIO status automantically in lightsleep.
        check_esp_err(gpio_sleep_sel_dis(self->pin));
        chans[self->mode][self->channel].lightsleep = true;
    }
    register_channel(self->mode, self->channel, self->pin, self->timer);
}

static uint32_t find_suitable_duty_resolution(uint32_t src_clk_freq, uint32_t timer_freq) {
    unsigned int resolution = ledc_find_suitable_duty_resolution(src_clk_freq, timer_freq);
    if (resolution > UI_RES_16_BIT) {
        // limit resolution to user interface
        resolution = UI_RES_16_BIT;
    }
    // Note: On ESP32, ESP32S2, ESP32S3, ESP32C3, ESP32C2, ESP32C6, ESP32H2, ESP32P4, due to a hardware bug,
    //       100% duty cycle (i.e. 2**duty_res) is not reachable when the binded timer selects the maximum duty
    //       resolution. For example, the max duty resolution on ESP32C3 is 14-bit width, then set duty to (2**14)
    //       will mess up the duty calculation in hardware.
    // Reduce the resolution from 14 to 13 bits to resolve the hardware bug.
    if (resolution >= SOC_LEDC_TIMER_BIT_WIDTH) {
        resolution -= 1;
    }
    return resolution;
}

static uint32_t get_duty_u16(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    int duty = ledc_get_duty(self->mode, self->channel) << (UI_RES_16_BIT - timers[self->mode][self->timer].duty_resolution);
    if (duty != MAX_16_DUTY) {
        return duty;
    } else {
        return MAX_16_DUTY - 1;
    }
}

static uint32_t get_duty_u10(machine_pwm_obj_t *self) {
    // Scale down from 16 bit to 10 bit resolution
    return get_duty_u16(self) >> (UI_RES_16_BIT - UI_RES_10_BIT);
}

static uint32_t get_duty_ns(machine_pwm_obj_t *self) {
    return duty_to_ns(self, get_duty_u16(self));
}

static void check_duty_u16(machine_pwm_obj_t *self, int duty) {
    if ((duty < 0) || (duty > MAX_16_DUTY - 1)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty_u16 must be from 0 to %d"), MAX_16_DUTY);
    }
    if (duty == MAX_16_DUTY - 1) {
        duty = MAX_16_DUTY;
    }
    self->duty_scale = DUTY_16;
    self->duty_ui = duty;
}

static void set_duty_u16(machine_pwm_obj_t *self, int duty) {
    check_duty_u16(self, duty);
    apply_duty(self);
}

static void check_duty_u10(machine_pwm_obj_t *self, int duty) {
    if ((duty < 0) || (duty > MAX_10_DUTY - 1)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty must be from 0 to %u"), MAX_10_DUTY - 1);
    }
    if (duty == MAX_10_DUTY - 1) {
        duty = MAX_10_DUTY;
    }
    self->duty_scale = DUTY_10;
    self->duty_ui = duty;
}

static void set_duty_u10(machine_pwm_obj_t *self, int duty) {
    check_duty_u10(self, duty);
    apply_duty(self);
}

static void check_duty_ns(machine_pwm_obj_t *self, int ns) {
    if ((ns < 0) || (ns > duty_to_ns(self, MAX_16_DUTY))) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty_ns must be from 0 to %d ns"), duty_to_ns(self, MAX_16_DUTY));
    }
    self->duty_scale = DUTY_NS;
    self->duty_ui = ns;
}

static void set_duty_ns(machine_pwm_obj_t *self, int ns) {
    check_duty_ns(self, ns);
    apply_duty(self);
}

#if !(CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)
// This check if a clock is already set in the timer list, if yes, return the LEDC_XXX value
static ledc_clk_cfg_t find_clock_in_use() {
    for (int mode = 0; mode < LEDC_SPEED_MODE_MAX; ++mode) {
        for (int timer = 0; timer < LEDC_TIMER_MAX; ++timer) {
            if (timers[mode][timer].clk_cfg != LEDC_AUTO_CLK) {
                return timers[mode][timer].clk_cfg;
            }
        }
    }
    return LEDC_AUTO_CLK;
}

// Check if a timer is already set with a different clock source
static bool is_timer_with_different_clock(int mode, int current_timer, ledc_clk_cfg_t clk_cfg) {
    for (int timer = 0; timer < LEDC_TIMER_MAX; ++timer) {
        if ((timer != current_timer) && (clk_cfg != LEDC_AUTO_CLK) && (timers[mode][timer].clk_cfg != LEDC_AUTO_CLK) && (timers[mode][timer].clk_cfg != clk_cfg)) {
            return true;
        }
    }
    return false;
}
#endif

static void check_freq_ranges(machine_pwm_obj_t *self, int freq, int upper_freq) {
    if ((freq <= 0) || (freq > upper_freq)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("frequency must be from 1Hz to %dMHz"), upper_freq / 1000000);
    }
}

// Set timer frequency
static void set_freq(machine_pwm_obj_t *self, unsigned int freq) {
    self->freq = freq;
    if ((timers[self->mode][self->timer].freq != freq) || (self->lightsleep)) {
        ledc_timer_config_t timer = {};
        timer.speed_mode = self->mode;
        timer.timer_num = self->timer;
        timer.freq_hz = freq;
        timer.deconfigure = false;

        timer.clk_cfg = LEDC_AUTO_CLK;
        #if !(CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)
        ledc_clk_cfg_t clk_cfg = find_clock_in_use();
        if (clk_cfg != LEDC_AUTO_CLK) {
            timer.clk_cfg = clk_cfg;
        } else
        #endif
        if (self->lightsleep) {
            timer.clk_cfg = LEDC_USE_RC_FAST_CLK; // 8 or 20 MHz
        } else {
            #if SOC_LEDC_SUPPORT_APB_CLOCK
            timer.clk_cfg = LEDC_USE_APB_CLK; // 80 MHz
            #elif SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
            timer.clk_cfg = LEDC_USE_PLL_DIV_CLK; // 60 or 80 or 96 MHz
            #elif SOC_LEDC_SUPPORT_XTAL_CLOCK
            timer.clk_cfg = LEDC_USE_XTAL_CLK; // 40 MHz
            #else
            #error No supported PWM / LEDC clocks.
            #endif

            #ifdef EMPIRIC_FREQ // ESP32 and ESP32S2 only
            if (freq < EMPIRIC_FREQ) {
                timer.clk_cfg = LEDC_USE_REF_TICK; // 1 MHz
            }
            #endif
        }
        #if !(CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)
        // Check for clock source conflict
        clk_cfg = find_clock_in_use();
        if ((clk_cfg != LEDC_AUTO_CLK) && (clk_cfg != timer.clk_cfg)) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("one or more active timers use a different clock source, not supported by the current SoC."));
        }
        #endif

        uint32_t src_clk_freq = 0;
        check_esp_err(esp_clk_tree_src_get_freq_hz(timer.clk_cfg, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &src_clk_freq));
        // machine.freq(20_000_000) reduces APB_CLK_FREQ to 20MHz and the highest PWM frequency to 10MHz
        check_freq_ranges(self, freq, src_clk_freq / 2);

        // Configure the new resolution
        timer.duty_resolution = find_suitable_duty_resolution(src_clk_freq, self->freq);
        // Configure timer - Set frequency
        if ((timers[self->mode][self->timer].duty_resolution == timer.duty_resolution) && (timers[self->mode][self->timer].clk_cfg == timer.clk_cfg)) {
            check_esp_err(ledc_set_freq(self->mode, self->timer, freq));
        } else {
            check_esp_err(ledc_timer_config(&timer));
        }
        // Reset the timer if low speed
        if (self->mode == LEDC_LOW_SPEED_MODE) {
            check_esp_err(ledc_timer_rst(self->mode, self->timer));
        }
        timers[self->mode][self->timer].freq = freq;
        timers[self->mode][self->timer].duty_resolution = timer.duty_resolution;
        timers[self->mode][self->timer].clk_cfg = timer.clk_cfg;
    }
}

static bool is_free_channels(int mode, int pin) {
    for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
        if ((chans[mode][channel].pin < 0) || (chans[mode][channel].pin == pin)) {
            return true;
        }
    }
    return false;
}

static bool is_free_timers(int mode, int32_t freq) {
    for (int timer = 0; timer < LEDC_TIMER_MAX; ++timer) {
        if ((timers[mode][timer].freq < 0) || (timers[mode][timer].freq == freq)) {
            return true;
        }
    }
    return false;
}

// Find self channel or free channel
static void find_channel(machine_pwm_obj_t *self, int *ret_mode, int *ret_channel, int32_t freq) {
    // Try to find self channel first
    for (int mode = 0; mode < LEDC_SPEED_MODE_MAX; ++mode) {
        #if SOC_LEDC_SUPPORT_HS_MODE
        if (self->lightsleep && (mode == LEDC_HIGH_SPEED_MODE)) {
            continue;
        }
        #endif
        for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
            if (chans[mode][channel].pin == self->pin) {
                *ret_mode = mode;
                *ret_channel = channel;
                return;
            }
        }
    }
    // Find free channel
    for (int mode = 0; mode < LEDC_SPEED_MODE_MAX; ++mode) {
        #if SOC_LEDC_SUPPORT_HS_MODE
        if (self->lightsleep && (mode == LEDC_HIGH_SPEED_MODE)) {
            continue;
        }
        #endif
        for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
            if ((chans[mode][channel].pin < 0) && is_free_timers(mode, freq)) {
                *ret_mode = mode;
                *ret_channel = channel;
                return;
            }
        }
    }
}

// Returns timer with the same frequency, freq == -1 means free timer
static void find_timer(machine_pwm_obj_t *self, int freq, int *ret_mode, int *ret_timer) {
    for (int mode = 0; mode < LEDC_SPEED_MODE_MAX; ++mode) {
        #if SOC_LEDC_SUPPORT_HS_MODE
        if (self->lightsleep && (mode == LEDC_HIGH_SPEED_MODE)) {
            continue;
        }
        #endif
        if (is_free_channels(mode, self->pin)) {
            for (int timer = 0; timer < LEDC_TIMER_MAX; ++timer) {
                if (timers[mode][timer].freq == freq) {
                    *ret_mode = mode;
                    *ret_timer = timer;
                    return;
                }
            }
        }
    }
}

// Try to find a timer with the same frequency in the current mode, otherwise in the next mode.
// If no existing timer and channel was found, then try to find free timer in any mode.
// If the mode or channel is changed, release the channel and register a new channel in the next mode.
static void select_timer(machine_pwm_obj_t *self, int freq) {
    // mode, channel, timer may be -1(not defined) or actual values
    int mode = -1;
    int timer = -1;
    // Check if an already running timer with the required frequency is running
    find_timer(self, freq, &mode, &timer);
    if (timer < 0) {
        // Try to reuse self timer
        if ((self->mode >= 0) && (self->channel >= 0)) {
            if (!is_timer_in_use(self->mode, self->channel, self->timer)) {
                mode = self->mode;
                timer = self->timer;
            }
        }
        // If no existing timer and channel was found, then try to find free timer in any mode
        if (timer < 0) {
            find_timer(self, -1, &mode, &timer);
        }
    }
    if (timer < 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("out of %sPWM timers:%d"), self->lightsleep ? "light sleep capable " : "", self->lightsleep ? LEDC_TIMER_MAX : LEDC_SPEED_MODE_MAX *LEDC_TIMER_MAX);
    }
    // If the timer is found, then register
    if (self->timer != timer) {
        unregister_channel(self->mode, self->channel);
        // Rregister the channel to the timer
        self->mode = mode;
        self->timer = timer;
        register_channel(self->mode, self->channel, -1, self->timer);
    }
    #if !(CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)
    if (is_timer_with_different_clock(self->mode, self->timer, timers[self->mode][self->timer].clk_cfg)) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("one or more active timers use a different clock source, not supported by the current SoC."));
    }
    #endif
}

static void set_freq_duty(machine_pwm_obj_t *self, unsigned int freq) {
    select_timer(self, freq);
    set_freq(self, freq);
    apply_duty(self);
}

// ******************************************************************************
// MicroPython bindings for PWM

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(Pin(%u)", self->pin);
    if (self->timer >= 0) {
        mp_printf(print, ", freq=%u", ledc_get_freq(self->mode, self->timer));
        if (self->duty_scale == DUTY_10) {
            mp_printf(print, ", duty=%d", get_duty_u10(self));
        } else if (self->duty_scale == DUTY_NS) {
            mp_printf(print, ", duty_ns=%d", get_duty_ns(self));
        } else {
            mp_printf(print, ", duty_u16=%d", get_duty_u16(self));
        }
        if (self->output_invert) {
            mp_printf(print, ", invert=True");
        }
        if (self->lightsleep) {
            mp_printf(print, ", lightsleep=True");
        }
        mp_printf(print, ")");

        #if MICROPY_ERROR_REPORTING > MICROPY_ERROR_REPORTING_NORMAL
        mp_printf(print, "  # duty=%.2f%%", 100.0 * get_duty_u16(self) / MAX_16_DUTY);
        mp_printf(print, ", raw_duty=%d, resolution=%d", ledc_get_duty(self->mode, self->channel), timers[self->mode][self->timer].duty_resolution);
        mp_printf(print, ", mode=%d, timer=%d, channel=%d", self->mode, self->timer, self->channel);
        int clk_cfg = timers[self->mode][self->timer].clk_cfg;
        mp_printf(print, ", clk_cfg=%d=", clk_cfg);
        if (clk_cfg == LEDC_USE_RC_FAST_CLK) {
            mp_printf(print, "RC_FAST_CLK");
        }
        #if SOC_LEDC_SUPPORT_APB_CLOCK
        else if (clk_cfg == LEDC_USE_APB_CLK) {
            mp_printf(print, "APB_CLK");
        }
        #endif
        #if SOC_LEDC_SUPPORT_XTAL_CLOCK
        else if (clk_cfg == LEDC_USE_XTAL_CLK) {
            mp_printf(print, "XTAL_CLK");
        }
        #endif
        #if SOC_LEDC_SUPPORT_REF_TICK
        else if (clk_cfg == LEDC_USE_REF_TICK) {
            mp_printf(print, "REF_TICK");
        }
        #endif
        #if SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
        else if (clk_cfg == LEDC_USE_PLL_DIV_CLK) {
            mp_printf(print, "PLL_CLK");
        }
        #endif
        else if (clk_cfg == LEDC_AUTO_CLK) {
            mp_printf(print, "AUTO_CLK");
        } else {
            mp_printf(print, "UNKNOWN");
        }
        #endif
    } else {
        mp_printf(print, ")");
    }
}

// This called from pwm.init() method
//
// Check the current mode.
// If the frequency is changed, try to find a timer with the same frequency
// in the current mode, otherwise in the new mode.
// If the mode is changed, release the channel and select a new channel in the new mode.
// Then set the frequency with the same duty.
static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_freq, ARG_duty, ARG_duty_u16, ARG_duty_ns, ARG_invert, ARG_lightsleep };
    mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = self->output_invert} },
        { MP_QSTR_lightsleep, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = self->lightsleep} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->lightsleep = args[ARG_lightsleep].u_bool;

    int freq = args[ARG_freq].u_int;
    if (freq != -1) {
        check_freq_ranges(self, freq, 40000000);
    }

    int duty = args[ARG_duty].u_int;
    int duty_u16 = args[ARG_duty_u16].u_int;
    int duty_ns = args[ARG_duty_ns].u_int;
    if (duty_u16 >= 0) {
        check_duty_u16(self, duty_u16);
    } else if (duty_ns >= 0) {
        check_duty_ns(self, duty_ns);
    } else if (duty >= 0) {
        check_duty_u10(self, duty);
    } else if (self->duty_scale == 0) {
        self->duty_scale = DUTY_16;
        self->duty_ui = PWM_DUTY;
    }

    self->output_invert = args[ARG_invert].u_bool;

    // Check the current mode and channel
    int mode = -1;
    int channel = -1;
    find_channel(self, &mode, &channel, freq);
    if (channel < 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("out of %sPWM channels:%d"), self->lightsleep ? "light sleep capable " : "", self->lightsleep ? LEDC_CHANNEL_MAX : LEDC_SPEED_MODE_MAX *LEDC_CHANNEL_MAX);
    }
    self->mode = mode;
    self->channel = channel;

    // Check if freq wasn't passed as an argument
    if ((freq == -1) && (mode >= 0) && (channel >= 0)) {
        // Check if already set, otherwise use the default freq.
        // It is possible in case:
        // pwm = PWM(pin, freq=1000, duty=256)
        // pwm = PWM(pin, duty=128)
        if (chans[mode][channel].timer >= 0) {
            freq = timers[mode][chans[mode][channel].timer].freq;
        }
        if (freq <= 0) {
            freq = PWM_FREQ;
        }
    }
    set_freq_duty(self, freq);
}

static void self_reset(machine_pwm_obj_t *self) {
    self->mode = -1;
    self->channel = -1;
    self->timer = -1;
    self->freq = -1;
    self->duty_scale = 0;
    self->duty_ui = 0;
    self->channel_duty = -1;
    self->output_invert = false;
    self->output_invert_prev = false;
    self->lightsleep = false;
}

// This called from PWM() constructor
static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, true);

    // start the PWM subsystem if it's not already running
    if (!pwm_inited) {
        pwm_init();
        #if FADE
        ledc_fade_func_install(0);
        #endif
        pwm_inited = true;
    }

    // create PWM object from the given pin
    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self_reset(self);

    self->pin = machine_pin_get_id(args[0]);

    // Process the remaining parameters.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);
    return MP_OBJ_FROM_PTR(self);
}

// This called from pwm.deinit() method
static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    pwm_deinit(self->mode, self->channel, self->output_invert);
    self_reset(self);
}

// Set and get methods of PWM class

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    return MP_OBJ_NEW_SMALL_INT(ledc_get_freq(self->mode, self->timer));
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    pwm_is_active(self);
    check_freq_ranges(self, freq, 40000000);
    if (freq == timers[self->mode][self->timer].freq) {
        return;
    }
    set_freq_duty(self, freq);
}

static mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(get_duty_u10(self));
}

static void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty) {
    set_duty_u10(self, duty);
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(get_duty_u16(self));
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    set_duty_u16(self, duty_u16);
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(get_duty_ns(self));
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    set_duty_ns(self, duty_ns);
}
