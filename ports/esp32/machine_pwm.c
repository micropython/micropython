/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Damien P. George
 * Copyright (c) 2018 Alan Dragomirecky
 * Copyright (c) 2020 Antoine Aubert
 * Copyright (c) 2021, 2023, 2024 Ihor Nehrutsa
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
#include "hal/ledc_hal.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "soc/gpio_sig_map.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
#include "esp_clk_tree.h"
#endif

#include "py/mpprint.h"

#define debug_printf(...) //mp_printf(&mp_plat_print, __VA_ARGS__); mp_printf(&mp_plat_print, ", LINE=%d\n", __LINE__);
#define debug_printf0(...)//  mp_printf(&mp_plat_print, __VA_ARGS__); mp_printf(&mp_plat_print, ", LINE=%d\n", __LINE__);
#define debug_printf2(...) //mp_printf(&mp_plat_print, __VA_ARGS__); mp_printf(&mp_plat_print, ", LINE=%d\n", __LINE__);
#define debug_printf3(...) //mp_printf(&mp_plat_print, __VA_ARGS__); mp_printf(&mp_plat_print, ", LINE=%d\n", __LINE__);
#define debug_printf4(...)//  mp_printf(&mp_plat_print, __VA_ARGS__); mp_printf(&mp_plat_print, ", LINE=%d\n", __LINE__);
#define debug_printf5(...) mp_printf(&mp_plat_print, __VA_ARGS__); mp_printf(&mp_plat_print, ", LINE=%d\n", __LINE__);

typedef struct _chan_t {
    // Which channel has which GPIO pin assigned?
    // (-1 if not assigned)
    gpio_num_t pin;
    // Which channel has which timer assigned?
    // (-1 if not assigned)
    int timer;
} chan_t;

// List of PWM channels
static chan_t chans[LEDC_SPEED_MODE_MAX][LEDC_CHANNEL_MAX];

// List of timer configs
static ledc_timer_config_t timers[LEDC_SPEED_MODE_MAX][LEDC_TIMER_MAX];

// Params for PWM operation
// 5khz is default frequency
#define PWM_FREQ (5000)

// 10-bit user interface resolution compatible with esp8266 PWM.duty(0..1024)
#define UI_RES_10_BIT (10)

// Maximum duty value on 10-bit resolution
#define UI10_DUTY (1 << UI_RES_10_BIT)

// Duty resolution of user interface in `duty_u16(0..2^16)` and `duty_u16` parameter in constructor/initializer
#define UI_RES_16_BIT (16)
// Maximum duty value on highest user interface resolution
#define UI16_DUTY (1 << UI_RES_16_BIT)

#define HIGHEST_PWM_RES (LEDC_TIMER_BIT_MAX - 1)

// timer_duty is the MAX value of a channel_duty
#define timer_duty (1 << timer->duty_resolution)

// All chips except esp32 and esp32s2 do not have timer-specific clock sources, which means clock source for all timers must be the same one.
#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
// If the PWM frequency is less than EMPIRIC_FREQ, then LEDC_REF_CLK_HZ(1 MHz) source is used, else LEDC_APB_CLK_HZ(80 MHz) source is used
#define EMPIRIC_FREQ (10) // Hz
#endif
// Config of timer upon which we run all PWM'ed GPIO pins
static bool pwm_inited = false;

// MicroPython PWM object struct
typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    gpio_num_t pin;
    int mode;
    int channel;
    int timer;
    int duty_x;       // UI_RES_10_BIT if duty(), UI_RES_16_BIT if duty_u16(), -UI_RES_16_BIT if duty_ns()
    int duty_ui;      // saved values of UI duty
    int channel_duty; // saved values of UI duty, calculated to raw channel->duty
    uint8_t output_invert;
} machine_pwm_obj_t;

static void register_channel(int mode, int channel, int pin, int timer) {
    if ((mode >= 0) && (mode < LEDC_SPEED_MODE_MAX) && (channel >= 0) && (channel < LEDC_CHANNEL_MAX)) {
        chans[mode][channel].pin = pin;
        chans[mode][channel].timer = timer;
    }
}

static void unregister_channel(int mode, int channel) {
    if ((mode >= 0) && (mode < LEDC_SPEED_MODE_MAX) && (channel >= 0) && (channel < LEDC_CHANNEL_MAX)) {
        chans[mode][channel].pin = -1;
        chans[mode][channel].timer = -1;
    }
}

static void pwm_init(void) {
    // Initial condition: no channels assigned
    for (int mode = 0; mode < LEDC_SPEED_MODE_MAX; ++mode) {
        for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
            unregister_channel(mode, channel);
        }

        // Prepare all timers config
        // Initial condition: no timers assigned
        for (int timer = 0; timer < LEDC_TIMER_MAX; ++timer) {
            timers[mode][timer].duty_resolution = HIGHEST_PWM_RES;
            timers[mode][timer].freq_hz = -1; // unset timer is -1
            timers[mode][timer].speed_mode = mode;
            timers[mode][timer].timer_num = timer;
            timers[mode][timer].clk_cfg = LEDC_AUTO_CLK; // will reinstall later
        }
    }
}

// Returns true if the timer is in use in addition to current channel
static bool is_timer_in_use(int mode, int current_channel, int timer) {
    for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
        if ((channel != current_channel) && (chans[mode][channel].timer == timer)) {
            return true;
        }
    }
    return false;
}

// Deinit channel and timer if the timer is unused, detach pin
static void pwm_deinit(int mode, int channel) {
    // Is valid channel?
    if ((mode >= 0) && (mode < LEDC_SPEED_MODE_MAX) && (channel >= 0) && (channel < LEDC_CHANNEL_MAX)) {
        // Clean up timer if necessary
        int timer = chans[mode][channel].timer;
        if (timer >= 0) {
            if (!is_timer_in_use(mode, channel, timer)) {
                check_esp_err(ledc_timer_rst(mode, timer));
                // Flag it unused
                timers[mode][timer].freq_hz = -1;
            }
        }

        int pin = chans[mode][channel].pin;
        if (pin >= 0) {
            // Mark it unused, and tell the hardware to stop routing
            check_esp_err(ledc_stop(mode, channel, 0));
            /*
            // Disable ledc signal for the pin
            if (mode == LEDC_LOW_SPEED_MODE) {
                esp_rom_gpio_connect_out_signal(pin, LEDC_LS_SIG_OUT0_IDX + channel, false, false);
            } else {
                #if SOC_LEDC_SUPPORT_HS_MODE
                esp_rom_gpio_connect_out_signal(pin, LEDC_HS_SIG_OUT0_IDX + channel, false, false);
                #endif
            }
            // reconfigure as GPIO
            //gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
            */
        }
        unregister_channel(mode, channel);
    }
}

// This called from Ctrl-D soft reboot
void machine_pwm_deinit_all(void) {
    if (pwm_inited) {
        for (int mode = 0; mode < LEDC_SPEED_MODE_MAX; ++mode) {
            for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
                pwm_deinit(mode, channel);
            }
        }
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
    ledc_timer_config_t *timer = &timers[self->mode][self->timer];
    int64_t duty = ((int64_t)ns * UI16_DUTY * timer->freq_hz + 500000000LL) / 1000000000LL;
    if ((ns > 0) && (duty == 0)) {
        duty = 1;
    } else if (duty > UI16_DUTY) {
        duty = UI16_DUTY;
    }
    return duty;
}

static int duty_to_ns(machine_pwm_obj_t *self, int duty) {
    pwm_is_active(self);
    ledc_timer_config_t *timer = &timers[self->mode][self->timer];
    int64_t ns = ((int64_t)duty * 1000000000LL + (int64_t)timer->freq_hz * UI16_DUTY / 2) / ((int64_t)timer->freq_hz * UI16_DUTY);
    return ns;
}

static void configure_pin(machine_pwm_obj_t *self) {
    // reconfigure PWM pin output as input/output
    gpio_set_direction(self->pin, GPIO_MODE_INPUT_OUTPUT);
    if (self->mode == LEDC_LOW_SPEED_MODE) {
        esp_rom_gpio_connect_out_signal(self->pin, LEDC_LS_SIG_OUT0_IDX + self->channel, self->output_invert, false);
    #if SOC_LEDC_SUPPORT_HS_MODE
    } else if (self->mode == LEDC_HIGH_SPEED_MODE) {
        esp_rom_gpio_connect_out_signal(self->pin, LEDC_HS_SIG_OUT0_IDX + self->channel, self->output_invert, false);
    #endif
    }
}

#define apply_duty(self) debug_printf0("apply_duty(self->channel=%d,self->channel_duty=%d,self->pin=%d,self->mode=%d,self->timer=%d)", self->channel,self->channel_duty,self->pin,self->mode,self->timer); _apply_duty(self) ;
static void _apply_duty(machine_pwm_obj_t *self) {
    ledc_timer_config_t *timer = &timers[self->mode][self->timer];

    int duty = 0;
    if (self->duty_x == UI_RES_16_BIT) {
        duty = self->duty_ui;
    } else if (self->duty_x == UI_RES_10_BIT) {
        duty = self->duty_ui << (UI_RES_16_BIT - UI_RES_10_BIT);
    } else if (self->duty_x == -UI_RES_16_BIT) {
        duty = ns_to_duty(self, self->duty_ui);
    }

    int channel_duty;
    if (timer->duty_resolution <= UI_RES_16_BIT) {
        channel_duty = duty >> (UI_RES_16_BIT - timer->duty_resolution);
    } else {
        channel_duty = duty << (timer->duty_resolution - UI_RES_16_BIT);
    }
    /*
    if (channel_duty < 0) {
        channel_duty = 0;
    } else if (channel_duty >= timer_duty) {
        // When channel's binded timer selects its maximum duty resolution,
        // the duty cycle value cannot be set to (2 ** duty_resolution)
        channel_duty = timer_duty - 1; // Ok!
    }
    */
    self->channel_duty = channel_duty;

    ledc_channel_config_t cfg = {
        .channel = self->channel,
        .duty = self->channel_duty,
        .gpio_num = self->pin,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = self->mode,
        .timer_sel = self->timer,
        .flags.output_invert = self->output_invert,
    };
    debug_printf4("apply_duty():self->channel_duty=%d, timer_duty=%d, cfg.flags.output_invert=%d", self->channel_duty, timer_duty, cfg.flags.output_invert)
    if (self->channel_duty == timer_duty) {
        cfg.duty = 0;
        cfg.flags.output_invert = self->output_invert ^ 1;
    }
    check_esp_err(ledc_channel_config(&cfg));
//    if (ledc_channel_config(&cfg) != ESP_OK) {
  //      mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM not supported on Pin(%d)"), self->pin);
    //}
}

static uint32_t calc_divider(uint32_t src_clk_freq, uint32_t timer_freq) {
    uint32_t divider = (uint32_t)(((uint64_t)src_clk_freq + (uint64_t)timer_freq / 2ULL) / (uint64_t)timer_freq); // rounded
    return divider == 0 ? 1 : divider;
}
/*
static uint32_t calc_divider(uint64_t src_clk_freq, uint64_t timer_freq) {
    uint32_t divider = (uint32_t)((src_clk_freq + timer_freq / 2ULL) / timer_freq); // rounded
    return divider == 0 ? 1 : divider;
}
*/

// Temporary workaround for ledc_find_suitable_duty_resolution function only being added in IDF V5.2
#if 1//ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
static uint32_t _ledc_find_suitable_duty_resolution(uint32_t src_clk_freq, uint32_t timer_freq) {
    // Find the highest bit resolution for the requested frequency
    uint32_t divider = calc_divider(src_clk_freq, timer_freq);
    // actual frequency
    uint32_t freq = (uint32_t)((uint64_t)src_clk_freq * (uint64_t)divider / (uint64_t)src_clk_freq);

    uint32_t resolution = 0;
    for (; freq > 1; freq >>= 1) {
        ++resolution;
    }
    debug_printf5("\nsrc_clk_freq=%d, timer_freq=%d, divider=%d, freq=%d, resolution=%d", src_clk_freq, timer_freq, divider, freq, resolution);
    return resolution;
}
#endif

static uint32_t find_suitable_duty_resolution(uint32_t src_clk_freq, uint32_t timer_freq) {
    unsigned int resolution;
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 2, 0)
    //resolution = ledc_find_suitable_duty_resolution(src_clk_freq, timer_freq);
    //if (timer_freq < 10)
    #endif
    {
        // magic second calculation
        resolution = _ledc_find_suitable_duty_resolution(src_clk_freq, timer_freq);
    }
    if (resolution == 0) {
        //resolution = 1;
    } else if (resolution > HIGHEST_PWM_RES) {
        // Limit resolution to HIGHEST_PWM_RES to match units of our duty
        resolution = HIGHEST_PWM_RES;
    }
    return resolution;
}

#define ledc_duty() ledc_get_duty(self->mode, self->channel)

static uint32_t get_duty_u16(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    ledc_timer_config_t *timer = &timers[self->mode][self->timer];
    debug_printf("MAX_TIMER_DUTY=%d timer->duty_resolution=%d self->channel_duty=%d ledc_duty()=%d", MAX_TIMER_DUTY, timer->duty_resolution, self->channel_duty, ledc_duty())

    debug_printf2("get_duty_u16():self->output_invert=%d, ledc_duty()=%d", self->output_invert, ledc_duty())
    //if ((self->channel_duty == timer_duty) || (ledc_duty() >= timer_duty)) {
    if (self->channel_duty == timer_duty) {
        return UI16_DUTY;
        //return self->output_invert == 0 ? UI16_DUTY : 0;
        //return HIGHEST_PWM_RES;
    } else {
        /*
        if ((self->output_invert) && (ledc_duty() == 0)) {
            return UI16_DUTY;
        }
        */
        int duty_resolution = timers[self->mode][self->timer].duty_resolution;
        debug_printf2("duty_resolution=%d", duty_resolution)
        if (duty_resolution <= UI_RES_16_BIT) {
            return ledc_duty() << (UI_RES_16_BIT - duty_resolution);
        } else {
            return ledc_duty() >> (duty_resolution - UI_RES_16_BIT);
        }
    }
}

static uint32_t get_duty_u10(machine_pwm_obj_t *self) {
    return get_duty_u16(self) >> (UI_RES_16_BIT - UI_RES_10_BIT); // Scale down from 16 bit to 10 bit resolution
}

static uint32_t get_duty_ns(machine_pwm_obj_t *self) {
    return duty_to_ns(self, get_duty_u16(self));
}

static void set_duty_u16(machine_pwm_obj_t *self, int duty) {
    if ((duty < 0) || (duty > UI16_DUTY)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty_u16 must be from 0 to %d"), UI16_DUTY);
    }

    self->duty_x = UI_RES_16_BIT;
    self->duty_ui = duty;
    apply_duty(self);
    /*
    #define USE_THREAD_SAFE_LEDC_API 0
    #if USE_THREAD_SAFE_LEDC_API
    check_esp_err(ledc_set_duty_and_update(self->mode, self->channel, channel_duty, timer_duty - 1));
    esp_err_t err = ESP_OK; // ledc_set_duty_and_update(self->mode, self->channel, channel_duty, timer_duty - 1);
    if (err != ESP_OK)
    #endif
    {
        check_esp_err(ledc_set_duty(self->mode, self->channel, channel_duty));
        check_esp_err(ledc_update_duty(self->mode, self->channel));
    }
    */
}

static void set_duty_u10(machine_pwm_obj_t *self, int duty) {
    if ((duty < 0) || (duty > UI10_DUTY)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty must be from 0 to %u"), UI10_DUTY);
    }
    // set_duty_u16(self, duty << (UI_RES_16_BIT - UI_RES_10_BIT));
    self->duty_x = UI_RES_10_BIT;
    self->duty_ui = duty;
    apply_duty(self);
}

static void set_duty_ns(machine_pwm_obj_t *self, int ns) {
    if ((ns < 0) || (ns > duty_to_ns(self, UI16_DUTY))) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty_ns must be from 0 to %d ns"), duty_to_ns(self, UI16_DUTY));
    }
    // set_duty_u16(self, ns_to_duty(self, ns));
    self->duty_x = -UI_RES_16_BIT;
    self->duty_ui = ns;
    apply_duty(self);
}

static void set_duty(machine_pwm_obj_t *self) {
    if (self->duty_x == UI_RES_16_BIT) {
        set_duty_u16(self, self->duty_ui);
    } else if (self->duty_x == UI_RES_10_BIT) {
        set_duty_u10(self, self->duty_ui);
    } else if (self->duty_x == -UI_RES_16_BIT) {
        set_duty_ns(self, self->duty_ui);
    }
}

// Set timer frequency
static void set_freq(machine_pwm_obj_t *self, unsigned int freq) {
    ledc_timer_config_t *timer = &timers[self->mode][self->timer];
    if (timer->freq_hz != freq) {
        timer->freq_hz = freq;
        timer->clk_cfg = LEDC_USE_APB_CLK; // LEDC_AUTO_CLK;
        uint32_t src_clk_freq = APB_CLK_FREQ;
/*
        #if SOC_LEDC_SUPPORT_APB_CLOCK
        timer->clk_cfg = LEDC_APB_CLK; // LEDC_USE_APB_CLK;
        src_clk_freq = APB_CLK_FREQ; // 80 MHz
        #elif SOC_LEDC_SUPPORT_PLL_DIV_CLOCK
        timer->clk_cfg = LEDC_USE_PLL_DIV_CLK;
        #elif SOC_LEDC_SUPPORT_XTAL_CLOCK
        timer->clk_cfg = LEDC_USE_XTAL_CLK;
        src_clk_freq = XTAL_CLK_FREQ; // 40 MHz
        #else
        #error No supported PWM / LEDC clocks.
        #endif

        #ifdef EMPIRIC_FREQ
        if (freq < EMPIRIC_FREQ) {
            #if SOC_LEDC_SUPPORT_REF_TICK
            timer->clk_cfg = LEDC_USE_REF_TICK;
            src_clk_freq = REF_CLK_FREQ; // 1 MHz
            #else
            #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 2)
            timer->clk_cfg = LEDC_USE_RC_FAST_CLK;
            src_clk_freq = SOC_CLK_RC_FAST_FREQ_APPROX; // 8.5 or 17.5 MHz
            #elif defined(XTAL_CLK_FREQ)
            timer->clk_cfg = LEDC_USE_XTAL_CLK;
            src_clk_freq = XTAL_CLK_FREQ; // 40MHz // 32MHz
            #endif
            #endif
        }
        #endif
*/
        debug_printf4("src_clk_freq=%d, timer->freq_hz=%d, timer->clk_cfg=%d", src_clk_freq, timer->freq_hz, timer->clk_cfg)
        #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
        esp_err_t err = ESP_OK;
        //esp_err_t err = esp_clk_tree_src_get_freq_hz(timer->clk_cfg, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &src_clk_freq);
//        esp_err_t err = esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_APB, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &src_clk_freq);
        if (err != ESP_OK) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("unable to query source clock frequency %d"), (int)timer->clk_cfg);
        }
        #endif

        // Configure the new resolution and frequency
        timer->duty_resolution = find_suitable_duty_resolution(src_clk_freq, timer->freq_hz);

        debug_printf5("src_clk_freq=%d, timer->freq_hz=%d, timer->clk_cfg=%d, timer->duty_resolution=%d", src_clk_freq, timer->freq_hz, timer->clk_cfg, timer->duty_resolution)

        // Configure timer - Set frequency
        check_esp_err(ledc_timer_config(timer));
        /*
        if (ESP_OK != ledc_timer_config(timer)) {
            uint32_t divider = calc_divider(src_clk_freq, timer->freq_hz);
            check_esp_err(ledc_timer_set(timer->speed_mode, timer->timer_num, divider, timer->duty_resolution, timer->clk_cfg));
        }
        */
        // Reset the timer if low speed
        if (self->mode == LEDC_LOW_SPEED_MODE) {
            check_esp_err(ledc_timer_rst(self->mode, self->timer));
        }

        // Save the same duty cycle when frequency is changed
        set_duty(self);
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

// Find self channel or free channel in the mode
static int find_channel(int mode, int pin) {
    int avail_channel = -1;
    for (int channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
        if (chans[mode][channel].pin == pin) {
            return channel;
        }
        if ((avail_channel < 0) && (chans[mode][channel].pin < 0)) {
            avail_channel = channel;
        }
    }
    return avail_channel;
}

// Returns timer with the same mode and frequency, freq == -1 means free timer
#define find_timer(mode, freq) _find_timer(mode, freq); debug_printf0("find_timer(mode=%d, freq=%d)", mode, freq);
static int _find_timer(int mode, unsigned int freq) {
    for (int timer = 0; timer < LEDC_TIMER_MAX; ++timer) {
        if (timers[mode][timer].freq_hz == freq) {
            return timer;
        }
    }
    return -1;
}

// Try to find a timer with the same frequency in the current mode, otherwise in the next mode.
// If no existing timer and channel was found, then try to find free timer in any mode.
// If the mode or channel is changed, release the channel and select(bind) a new channel in the next mode.
#define select_a_timer(self, freq) debug_printf0("select_a_timer(freq=%d)", freq); _select_a_timer(self, freq);
static void _select_a_timer(machine_pwm_obj_t *self, int freq) {
    // mode, channel, timer may be -1(not defined) or actual values
    int save_mode = self->mode;
    int save_channel = self->channel;
    // int save_timer = self->timer;

    int mode = MAX(self->mode, 0);

    // Check if an already running timer with the required frequency is running in the current mode
    int timer = -1;
    if (is_free_channels(mode, self->pin)) {
        timer = find_timer(mode, freq);
    }
    // If no existing timer and channel was found in the current mode, then find a new one in another mode
    if (timer < 0) {
        // Calc next mode
        int mode_ = mode;
        if (mode > 0) {
            --mode;
        } else if (mode < (LEDC_SPEED_MODE_MAX - 1)) {
            ++mode;
        }

        if (mode_ != mode) {
            if (is_free_channels(mode, self->pin)) {
                timer = find_timer(mode, freq);
            }
        }
    }
    // If the timer is found, then bind and set the duty
    debug_printf0("timer=%d, timers[mode][timer].freq_hz=%d, self->channel=%d, self->mode=%d", timer, timers[mode][timer].freq_hz, self->channel, self->mode);
    if ((timer >= 0)
        && (timers[mode][timer].freq_hz != 0)
        && (timers[mode][timer].freq_hz != freq)
        && (self->channel >= 0)
        && (self->mode >= 0)) {
        // Bind the channel to the timer
        self->mode = mode;
        self->timer = timer;
        check_esp_err(ledc_bind_channel_timer(self->mode, self->channel, self->timer));
        register_channel(self->mode, self->channel, self->pin, self->timer);
    } else {
        ////// timer = -1;
    }
    if (timer >= 0) {
        self->timer = timer;
        set_duty(self);
    }

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
            mode = -1;
            while ((timer < 0) && (mode < (LEDC_SPEED_MODE_MAX - 1))) {
                ++mode;
                if (is_free_channels(mode, self->pin)) {
                    timer = find_timer(mode, -1); // find free timer
                }
            }
            if (timer < 0) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of PWM timers:%d"), LEDC_SPEED_MODE_MAX * LEDC_TIMER_MAX);
            }
        }
        self->mode = mode;
        self->timer = timer;
    }
    if ((save_mode != self->mode) || (save_channel != self->channel)) {
        unregister_channel(save_mode, save_channel);
    }
}

/******************************************************************************/
// MicroPython bindings for PWM

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(Pin(%u)", self->pin);
    if (self->timer >= 0) {
        mp_printf(print, ", freq=%u", ledc_get_freq(self->mode, self->timer));

        if (self->duty_x == UI_RES_10_BIT) {
            mp_printf(print, ", duty=%d", get_duty_u10(self));
        } else if (self->duty_x == -UI_RES_16_BIT) {
            mp_printf(print, ", duty_ns=%d", get_duty_ns(self));
        } else {
            mp_printf(print, ", duty_u16=%d", get_duty_u16(self));
        }
        if (self->output_invert) {
            mp_printf(print, ", invert=%d", self->output_invert);
        }
        mp_printf(print, ")");
        #if 1//MICROPY_ERROR_REPORTING > MICROPY_ERROR_REPORTING_NORMAL
        int duty_resolution = timers[self->mode][self->timer].duty_resolution;
        mp_printf(print, "  #");
        mp_printf(print, " duty=%.2f%%", 100.0 * get_duty_u16(self) / UI16_DUTY); // percents
        mp_printf(print, ", raw_duty()=%d, duty_resolution=%d", ledc_duty(), duty_resolution);
        mp_printf(print, ", mode=%d, channel=%d, timer=%d", self->mode, self->channel, self->timer);
        #endif
    } else {
        mp_printf(print, ")");
    }
}

// This called from pwm.init() method
/*
Check the current mode.
If the frequency is changed, try to find a timer with the same frequency
in the current mode, otherwise in the new mode.
If the mode is changed, release the channel and select a new channel in the new mode.
Then set the frequency with the same duty.
*/
static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    enum { ARG_freq, ARG_duty, ARG_duty_u16, ARG_duty_ns, ARG_invert };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int freq = args[ARG_freq].u_int;
    if (freq != -1) {
        if ((freq <= 0) || (freq > 40000000)) {
            mp_raise_ValueError(MP_ERROR_TEXT("frequency must be from 1Hz to 40MHz"));
        }
    }

    int duty = args[ARG_duty].u_int;
    int duty_u16 = args[ARG_duty_u16].u_int;
    int duty_ns = args[ARG_duty_ns].u_int;
    /*
    if (((duty != -1) && (duty_u16 != -1)) || ((duty != -1) && (duty_ns != -1)) || ((duty_u16 != -1) && (duty_ns != -1))) {
        mp_raise_ValueError(MP_ERROR_TEXT("only one of parameters 'duty', 'duty_u16' or 'duty_ns' is allowed"));
    }
    */
    /*
    if ((duty < 0) && (duty_u16 < 0) && (duty_ns < 0)) {
        mp_raise_ValueError(MP_ERROR_TEXT("one of parameters 'duty', 'duty_u16', or 'duty_ns' is required"));
    }
    */
    if (duty_u16 >= 0) {
        self->duty_x = UI_RES_16_BIT;
        self->duty_ui = duty_u16;
    } else if (duty_ns >= 0) {
        self->duty_x = -UI_RES_16_BIT;
        self->duty_ui = duty_ns;
    } else if (duty >= 0) {
        self->duty_x = UI_RES_10_BIT;
        self->duty_ui = duty;
    } else if (self->duty_x == 0) {
        self->duty_x = UI_RES_16_BIT;
        self->duty_ui = (1 << UI_RES_16_BIT) / 2; // default 50%
    }

    int output_invert = args[ARG_invert].u_int;
    if (output_invert >= 0) {
        self->output_invert = output_invert == 0 ? 0 : 1;
    }

    int save_mode = self->mode;
    int save_channel = self->channel;
    int save_timer = self->timer;

    // Check the current mode and channel
    int mode = -1;
    int channel = -1;
    while ((channel < 0) && (mode < (LEDC_SPEED_MODE_MAX - 1))) {
        ++mode;
        channel = find_channel(mode, self->pin);
    }
    if (channel < 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of PWM channels:%d"), LEDC_SPEED_MODE_MAX * LEDC_CHANNEL_MAX); // in all modes
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
            freq = timers[mode][chans[mode][channel].timer].freq_hz;
        }
        if (freq <= 0) {
            freq = PWM_FREQ;
        }
    }

    select_a_timer(self, freq);

    // New PWM assignment
    if ((chans[mode][channel].pin < 0)
        || ((save_mode != self->mode))
        || ((save_channel != self->channel))
        || ((save_timer != self->timer))) {
        debug_printf("mp_machine_pwm_init_helper()")
        apply_duty(self);
        configure_pin(self);
        register_channel(self->mode, self->channel, self->pin, self->timer);
    } else {
        set_freq(self, freq);
    }
}

static void self_reset(machine_pwm_obj_t *self) {
    self->mode = -1;
    self->channel = -1;
    self->timer = -1;
    self->duty_x = 0;
    self->duty_ui = 0;
    self->channel_duty = 0;
    self->output_invert = 0;
}

// This called from PWM() constructor
static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, true);
    gpio_num_t pin = machine_pin_get_id(args[0]);

    // create PWM object from the given pin
    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->pin = pin;
    self_reset(self);

    // start the PWM subsystem if it's not already running
    if (!pwm_inited) {
        #if USE_THREAD_SAFE_LEDC_API
        ledc_fade_func_install(ESP_INTR_FLAG_INTRDISABLED);
        #endif
        pwm_init();
        pwm_inited = true;
    }

    // start the PWM running for this channel
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

// This called from pwm.deinit() method
static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    pwm_deinit(self->mode, self->channel);
    self_reset(self);
}

// Set and get methods of PWM class

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    if (self->timer < 0) {
        return MP_OBJ_NEW_SMALL_INT(0);
    } else {
        return MP_OBJ_NEW_SMALL_INT(ledc_get_freq(self->mode, self->timer));
    }
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    if ((freq <= 0) || (freq > 40000000)) {
        mp_raise_ValueError(MP_ERROR_TEXT("frequency must be from 1Hz to 40MHz"));
    }
    pwm_is_active(self);
    if (freq == timers[self->mode][self->timer].freq_hz) {
        return;
    }
    // Set new PWM frequency
    select_a_timer(self, freq);
    // Set the frequency
    set_freq(self, freq);
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
