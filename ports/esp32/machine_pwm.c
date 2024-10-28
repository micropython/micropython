/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Damien P. George
 * Copyright (c) 2018 Alan Dragomirecky
 * Copyright (c) 2020 Antoine Aubert
 * Copyright (c) 2021 Ihor Nehrutsa
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
#include "driver/ledc.h"
#include "esp_err.h"
#include "soc/gpio_sig_map.h"
// YDE
#include "esp_sleep.h"
#include "esp_clk_tree.h"
#include "soc/clk_tree_defs.h"

//#include "driver/rtc_io.h"
//#include "soc/rtc.h"

#define PWM_DBG(...)
// #define PWM_DBG(...) mp_printf(&mp_plat_print, __VA_ARGS__); mp_printf(&mp_plat_print, "\n");

// Total number of channels
#define PWM_CHANNEL_MAX (LEDC_SPEED_MODE_MAX * LEDC_CHANNEL_MAX)

// Only ESP32 or ESP32S2 can have independante clock source
#define PWM_SUPPORT_INDEP_CLOCK_SRC (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)

typedef struct _chan_t {
    // Which channel has which GPIO pin assigned?
    // (-1 if not assigned)
    gpio_num_t pin;
    // Which channel has which timer assigned?
    // (-1 if not assigned)
    int timer_idx;
    // Is light sleep enable has been set for this pin
    bool lightsleepenabled;
} chan_t;

// List of PWM channels
static chan_t chans[PWM_CHANNEL_MAX];

// channel_idx is an index (end-to-end sequential numbering) for all channels
// available on the chip and described in chans[]
#define CHANNEL_IDX(mode, channel) (mode * LEDC_CHANNEL_MAX + channel)
#define CHANNEL_IDX_TO_MODE(channel_idx) (channel_idx / LEDC_CHANNEL_MAX)
#define CHANNEL_IDX_TO_CHANNEL(channel_idx) (channel_idx % LEDC_CHANNEL_MAX)

// Total number of timers
#define PWM_TIMER_MAX (LEDC_SPEED_MODE_MAX * LEDC_TIMER_MAX)

// List of timer configs
static ledc_timer_config_t timers[PWM_TIMER_MAX];

// timer_idx is an index (end-to-end sequential numbering) for all timers
// available on the chip and configured in timers[]
#define TIMER_IDX(mode, timer) (mode * LEDC_TIMER_MAX + timer)
#define TIMER_IDX_TO_MODE(timer_idx) (timer_idx / LEDC_TIMER_MAX)
#define TIMER_IDX_TO_TIMER(timer_idx) (timer_idx % LEDC_TIMER_MAX)

// Params for PWM operation
// 5khz is default frequency
#define PWM_FREQ (5000)

// 10-bit resolution (compatible with esp8266 PWM)
#define PWM_RES_10_BIT (LEDC_TIMER_10_BIT)

// Maximum duty value on 10-bit resolution
#define MAX_DUTY_U10 ((1 << PWM_RES_10_BIT) - 1)
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html#supported-range-of-frequency-and-duty-resolutions
// duty() uses 10-bit resolution or less
// duty_u16() and duty_ns() use 16-bit resolution or less

// Possible highest resolution in device
#if (LEDC_TIMER_BIT_MAX - 1) < LEDC_TIMER_16_BIT
#define HIGHEST_PWM_RES (LEDC_TIMER_BIT_MAX - 1)
#else
#define HIGHEST_PWM_RES (LEDC_TIMER_16_BIT) // 20 bit for ESP32, but 16 bit is used
#endif
// Duty resolution of user interface in `duty_u16()` and `duty_u16` parameter in constructor/initializer
#define UI_RES_16_BIT (16)
// Maximum duty value on highest user interface resolution
#define UI_MAX_DUTY ((1 << UI_RES_16_BIT) - 1)
// How much to shift from the HIGHEST_PWM_RES duty resolution to the user interface duty resolution UI_RES_16_BIT
#define UI_RES_SHIFT (UI_RES_16_BIT - HIGHEST_PWM_RES) // 0 for ESP32, 2 for S2, S3, C3

#if SOC_LEDC_SUPPORT_REF_TICK
// If the PWM frequency is less than EMPIRIC_FREQ, then LEDC_REF_CLK_HZ(1 MHz) source is used, else LEDC_APB_CLK_HZ(80 MHz) source is used
#define EMPIRIC_FREQ (10) // Hz
#endif

// Clock alias values (used by clock parameter)
// PWM_LAST_CLK_IDX is not clock by a maker to identify outofindex values
// PWM_AUTO_CLK is used in order to auto determinate the clock (no specific clock has been required)
enum { PWM_APB_CLK, PWM_RC_FAST_CLK, PWM_REF_TICK, PWM_XTAL_CLK, PWM_AUTO_CLK, _PWM_LAST_CLK_IDX };
static const ledc_clk_cfg_t clk_source_map[] = {
    LEDC_USE_APB_CLK, 
    LEDC_USE_RC_FAST_CLK, // LEDC_USE_RC_FAST_CLK == LEDC_USE_RTC8M_CLK
    #if SOC_LEDC_SUPPORT_REF_TICK
    LEDC_USE_REF_TICK, 
    #else
    -1,
    #endif
    #if SOC_LEDC_SUPPORT_XTAL_CLOCK
    LEDC_USE_XTAL_CLK 
    #else
    -1
    #endif
};

// Config of timer upon which we run all PWM'ed GPIO pins
static bool pwm_inited = false;

// MicroPython PWM object struct
typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    gpio_num_t pin;
    bool active;
    bool lightsleepenabled;
    int mode;
    int channel;
    int timer;
    int duty_x; // PWM_RES_10_BIT if duty(), HIGHEST_PWM_RES if duty_u16(), -HIGHEST_PWM_RES if duty_ns()
    int duty_u10; // stored values from previous duty setters
    int duty_u16; // - / -
    int duty_ns; // - / -
} machine_pwm_obj_t;

static bool is_timer_in_use(int current_channel_idx, int timer_idx);
static void set_duty_u16(machine_pwm_obj_t *self, int duty);
static void set_duty_u10(machine_pwm_obj_t *self, int duty);
static void set_duty_ns(machine_pwm_obj_t *self, int ns);

static void pwm_init(void) {

    // Initial condition: no channels assigned
    for (int i = 0; i < PWM_CHANNEL_MAX; ++i) {
        chans[i].pin = -1;
        chans[i].timer_idx = -1;
        chans[i].lightsleepenabled = false;
    }

    // Prepare all timers config
    // Initial condition: no timers assigned
    for (int i = 0; i < PWM_TIMER_MAX; ++i) {
        timers[i].duty_resolution = HIGHEST_PWM_RES;
        // unset timer is -1
        timers[i].freq_hz = -1;
        timers[i].speed_mode = TIMER_IDX_TO_MODE(i);
        timers[i].timer_num = TIMER_IDX_TO_TIMER(i);
        timers[i].clk_cfg = LEDC_AUTO_CLK; // will reinstall later according to the EMPIRIC_FREQ or clock parameter
        //timers[i].clk_cfg = LEDC_USE_RC_FAST_CLK; 
    }
}

// Deinit channel and timer if the timer is unused
static void pwm_deinit(int channel_idx) {
    // Valid channel?
    if ((channel_idx >= 0) && (channel_idx < PWM_CHANNEL_MAX)) {
        // Clean up timer if necessary
        int timer_idx = chans[channel_idx].timer_idx;
        if (timer_idx != -1) {
            if (!is_timer_in_use(channel_idx, timer_idx)) {
                check_esp_err(ledc_timer_rst(TIMER_IDX_TO_MODE(timer_idx), TIMER_IDX_TO_TIMER(timer_idx)));
                // Flag it unused
                timers[chans[channel_idx].timer_idx].freq_hz = -1;
                timers[chans[channel_idx].timer_idx].clk_cfg = LEDC_AUTO_CLK;
            }
        }

        int pin = chans[channel_idx].pin;
        if (pin != -1) {
            int mode = CHANNEL_IDX_TO_MODE(channel_idx);
            int channel = CHANNEL_IDX_TO_CHANNEL(channel_idx);

            if (chans[channel_idx].lightsleepenabled) {
                // Enable SLP_SEL to change GPIO status automantically in lightsleep. 
                check_esp_err(gpio_sleep_sel_en(pin));
            }
        
            // Mark it unused, and tell the hardware to stop routing
            check_esp_err(ledc_stop(mode, channel, 0));
            // Disable ledc signal for the pin
            // esp_rom_gpio_connect_out_signal(pin, SIG_GPIO_OUT_IDX, false, false);
            if (mode == LEDC_LOW_SPEED_MODE) {
                esp_rom_gpio_connect_out_signal(pin, LEDC_LS_SIG_OUT0_IDX + channel, false, true);
            } else {
                #if LEDC_SPEED_MODE_MAX > 1
                #if CONFIG_IDF_TARGET_ESP32
                esp_rom_gpio_connect_out_signal(pin, LEDC_HS_SIG_OUT0_IDX + channel, false, true);
                #else
                #error Add supported CONFIG_IDF_TARGET_ESP32_xxx
                #endif
                #endif
            }
        }
        chans[channel_idx].pin = -1;
        chans[channel_idx].timer_idx = -1;
        chans[channel_idx].lightsleepenabled = false;
    }
}

// This called from Ctrl-D soft reboot
void machine_pwm_deinit_all(void) {
    if (pwm_inited) {
        for (int channel_idx = 0; channel_idx < PWM_CHANNEL_MAX; ++channel_idx) {
            pwm_deinit(channel_idx);
        }
        pwm_inited = false;
    }
}

static void configure_channel(machine_pwm_obj_t *self) {
    ledc_channel_config_t cfg = {
        .channel = self->channel,
        .duty = (1 << (timers[TIMER_IDX(self->mode, self->timer)].duty_resolution)) / 2,
        .gpio_num = self->pin,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = self->mode,
        .timer_sel = self->timer
    };
    if (ledc_channel_config(&cfg) != ESP_OK) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM not supported on Pin(%d)"), self->pin);
    }

    if(self->lightsleepenabled) {
        // Disable SLP_SEL to change GPIO status automantically in lightsleep. 
        check_esp_err(gpio_sleep_sel_dis(self->pin));
    }

}

// led_src_clock = LEDC_USE_APB_CLK, LEDC_USE_RC_FAST_CLK ...
static void set_freq(machine_pwm_obj_t *self, unsigned int freq, ledc_timer_config_t *timer, ledc_clk_cfg_t led_src_clock) {
    if (freq != timer->freq_hz) {

        uint32_t src_clock_freq;

        if ( esp_clk_tree_src_get_freq_hz(led_src_clock, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &src_clock_freq) != ESP_OK ) {
            mp_raise_ValueError(MP_ERROR_TEXT("Error on getting reference clock frequency."));
        }        

        uint32_t res = ledc_find_suitable_duty_resolution(src_clock_freq, freq);
        if(res > HIGHEST_PWM_RES) {
            res = HIGHEST_PWM_RES;
        }

        // Configure the new resolution and frequency
        timer->duty_resolution = res;
        timer->freq_hz = freq;
        timer->clk_cfg = led_src_clock;

        // Set frequency
        esp_err_t err = ledc_timer_config(timer);
        if (err != ESP_OK) {

            timer->freq_hz = -1;
            timer->clk_cfg = LEDC_AUTO_CLK; 

            if (err == ESP_FAIL) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("unreachable frequency %d"), freq);
            } else {
                check_esp_err(err);
            }
        }

        // Reset the timer if low speed
        if (self->mode == LEDC_LOW_SPEED_MODE) {
            check_esp_err(ledc_timer_rst(self->mode, self->timer));
        }
    }

    // Save the same duty cycle when frequency is changed
    if (self->duty_x == HIGHEST_PWM_RES) {
        set_duty_u16(self, self->duty_u16);
    } else if (self->duty_x == PWM_RES_10_BIT) {
        set_duty_u10(self, self->duty_u10);
    } else if (self->duty_x == -HIGHEST_PWM_RES) {
        set_duty_ns(self, self->duty_ns);
    }
}

// Calculate the duty parameters based on an ns value
static int ns_to_duty(machine_pwm_obj_t *self, int ns) {
    ledc_timer_config_t timer = timers[TIMER_IDX(self->mode, self->timer)];
    int64_t duty = ((int64_t)ns * UI_MAX_DUTY * timer.freq_hz + 500000000LL) / 1000000000LL;
    if ((ns > 0) && (duty == 0)) {
        duty = 1;
    } else if (duty > UI_MAX_DUTY) {
        duty = UI_MAX_DUTY;
    }
    return duty;
}

static int duty_to_ns(machine_pwm_obj_t *self, int duty) {
    ledc_timer_config_t timer = timers[TIMER_IDX(self->mode, self->timer)];
    int64_t ns = ((int64_t)duty * 1000000000LL + (int64_t)timer.freq_hz * UI_MAX_DUTY / 2) / ((int64_t)timer.freq_hz * UI_MAX_DUTY);
    return ns;
}

#define get_duty_raw(self) ledc_get_duty(self->mode, self->channel)

static void pwm_is_active(machine_pwm_obj_t *self) {
    if (self->active == false) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("PWM inactive"));
    }
}

static uint32_t get_duty_u16(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    int resolution = timers[TIMER_IDX(self->mode, self->timer)].duty_resolution;
    int duty = ledc_get_duty(self->mode, self->channel);
    if (resolution <= UI_RES_16_BIT) {
        duty <<= (UI_RES_16_BIT - resolution);
    } else {
        duty >>= (resolution - UI_RES_16_BIT);
    }
    return duty;
}

static uint32_t get_duty_u10(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    return get_duty_u16(self) >> 6; // Scale down from 16 bit to 10 bit resolution
}

static uint32_t get_duty_ns(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    return duty_to_ns(self, get_duty_u16(self));
}

static void set_duty_u16(machine_pwm_obj_t *self, int duty) {
    pwm_is_active(self);
    if ((duty < 0) || (duty > UI_MAX_DUTY)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty_u16 must be from 0 to %d"), UI_MAX_DUTY);
    }
    ledc_timer_config_t timer = timers[TIMER_IDX(self->mode, self->timer)];
    int channel_duty;
    if (timer.duty_resolution <= UI_RES_16_BIT) {
        channel_duty = duty >> (UI_RES_16_BIT - timer.duty_resolution);
    } else {
        channel_duty = duty << (timer.duty_resolution - UI_RES_16_BIT);
    }
    int max_duty = (1 << timer.duty_resolution) - 1;
    if (channel_duty < 0) {
        channel_duty = 0;
    } else if (channel_duty > max_duty) {
        channel_duty = max_duty;
    }
    check_esp_err(ledc_set_duty(self->mode, self->channel, channel_duty));
    check_esp_err(ledc_update_duty(self->mode, self->channel));

    /*
    // Bug: Sometimes duty is not set right now.
    // Not a bug. It's a feature. The duty is applied at the beginning of the next signal period.
    // Bug: It has been experimentally established that the duty is set during 2 signal periods, but 1 period is expected.
    // See https://github.com/espressif/esp-idf/issues/7288
    if (duty != get_duty_u16(self)) {
        PWM_DBG("set_duty_u16(%u), get_duty_u16():%u, channel_duty:%d, duty_resolution:%d, freq_hz:%d", duty, get_duty_u16(self), channel_duty, timer.duty_resolution, timer.freq_hz);
        esp_rom_delay_us(2 * 1000000 / timer.freq_hz);
        if (duty != get_duty_u16(self)) {
            PWM_DBG("set_duty_u16(%u), get_duty_u16():%u, channel_duty:%d, duty_resolution:%d, freq_hz:%d", duty, get_duty_u16(self), channel_duty, timer.duty_resolution, timer.freq_hz);
        }
    }
    */

    self->duty_x = HIGHEST_PWM_RES;
    self->duty_u16 = duty;
}

static void set_duty_u10(machine_pwm_obj_t *self, int duty) {
    pwm_is_active(self);
    if ((duty < 0) || (duty > MAX_DUTY_U10)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty must be from 0 to %u"), MAX_DUTY_U10);
    }
    set_duty_u16(self, duty << (UI_RES_16_BIT - PWM_RES_10_BIT));
    self->duty_x = PWM_RES_10_BIT;
    self->duty_u10 = duty;
}

static void set_duty_ns(machine_pwm_obj_t *self, int ns) {
    pwm_is_active(self);
    if ((ns < 0) || (ns > duty_to_ns(self, UI_MAX_DUTY))) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty_ns must be from 0 to %d ns"), duty_to_ns(self, UI_MAX_DUTY));
    }
    set_duty_u16(self, ns_to_duty(self, ns));
    self->duty_x = -HIGHEST_PWM_RES;
    self->duty_ns = ns;
}

/******************************************************************************/

#define SAME_FREQ_ONLY (true)
#define SAME_FREQ_OR_FREE (false)
#define ANY_MODE (-1)

// Return timer_idx. Use TIMER_IDX_TO_MODE(timer_idx) and TIMER_IDX_TO_TIMER(timer_idx) to get mode and timer
// Find the timer with the same freq and clock_source : {LEDC_USE_APB_CLK, LEDC_USE_RC_FAST_CLK, ... }
static int find_timer(unsigned int freq, bool same_freq_only, int mode, ledc_clk_cfg_t clock_source) {
    int free_timer_idx_found = -1;
    // Find a free PWM Timer using the same freq
    for (int timer_idx = 0; timer_idx < PWM_TIMER_MAX; ++timer_idx) {
        if ((mode == ANY_MODE) || (mode == TIMER_IDX_TO_MODE(timer_idx))) {
            if ((timers[timer_idx].freq_hz == freq) && (timers[timer_idx].clk_cfg == clock_source)) {
                // A timer already uses the same freq. Use it now.
                return timer_idx;
            }
            if (!same_freq_only && (free_timer_idx_found == -1) && (timers[timer_idx].freq_hz == -1)) {
                free_timer_idx_found = timer_idx;
                // Continue to check if a channel with the same freq is in use.
            }
        }
    }

    return free_timer_idx_found;
}

// Return true if the timer is in use in addition to current channel
static bool is_timer_in_use(int current_channel_idx, int timer_idx) {
    for (int i = 0; i < PWM_CHANNEL_MAX; ++i) {
        if ((i != current_channel_idx) && (chans[i].timer_idx == timer_idx)) {
            return true;
        }
    }

    return false;
}

#if !(PWM_SUPPORT_INDEP_CLOCK_SRC)
// Check if a timer is already set with a different clock source
static int is_timer_with_different_clock(int current_timer_idx, ledc_clk_cfg_t ref_clk_src) {
    for(int i=0; i < PWM_TIMER_MAX; i++) {
        if ( (i != current_timer_idx) && (timers[i].clk_cfg != LEDC_AUTO_CLK) && (timers[i].clk_cfg != ref_clk_src)) {
            return true;
        }
    }

    return false;
}

// This check if a clock is already set in the timer list, if yes, return the PWM_XXX_CLK value
static int find_clock_in_use() {

    ledc_clk_cfg_t found_clk = LEDC_AUTO_CLK;

    for(int i=0; i < PWM_TIMER_MAX; i++) {
        if (timers[i].clk_cfg != LEDC_AUTO_CLK)  {
            found_clk = timers[i].clk_cfg;
            break;
        }
    }

    if (found_clk == LEDC_AUTO_CLK) {
        return PWM_AUTO_CLK;
    } else if (found_clk == LEDC_USE_APB_CLK) {
        return PWM_APB_CLK;
    } else if (found_clk == LEDC_USE_RC_FAST_CLK) {
        return PWM_RC_FAST_CLK;        
    } 
    #if SOC_LEDC_SUPPORT_REF_TICK
    else if (found_clk == LEDC_USE_REF_TICK) {
        return PWM_REF_TICK;
    } 
    #endif
    #if SOC_LEDC_SUPPORT_XTAL_CLOCK
    else if (found_clk == LEDC_USE_XTAL_CLK) {
        return PWM_XTAL_CLK;
    }
    #endif

    return PWM_AUTO_CLK;
}

#endif

// Find a free PWM channel, also spot if our pin is already mentioned.
// Return channel_idx. Use CHANNEL_IDX_TO_MODE(channel_idx) and CHANNEL_IDX_TO_CHANNEL(channel_idx) to get mode and channel
static int find_channel(int pin, int mode) {
    int avail_idx = -1;
    int channel_idx;
    for (channel_idx = 0; channel_idx < PWM_CHANNEL_MAX; ++channel_idx) {
        if ((mode == ANY_MODE) || (mode == CHANNEL_IDX_TO_MODE(channel_idx))) {
            if (chans[channel_idx].pin == pin) {
                break;
            }
            if ((avail_idx == -1) && (chans[channel_idx].pin == -1)) {
                avail_idx = channel_idx;
            }
        }
    }
    if (channel_idx >= PWM_CHANNEL_MAX) {
        channel_idx = avail_idx;
    }
    return channel_idx;
}

/******************************************************************************/
// MicroPython bindings for PWM

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(Pin(%u)", self->pin);
    if (self->active) {
        mp_printf(print, ", freq=%u", ledc_get_freq(self->mode, self->timer));

        if (self->duty_x == PWM_RES_10_BIT) {
            mp_printf(print, ", duty=%d", get_duty_u10(self));
        } else if (self->duty_x == -HIGHEST_PWM_RES) {
            mp_printf(print, ", duty_ns=%d", get_duty_ns(self));
        } else {
            mp_printf(print, ", duty_u16=%d", get_duty_u16(self));
        }
        int resolution = timers[TIMER_IDX(self->mode, self->timer)].duty_resolution;
        mp_printf(print, ", resolution=%d", resolution);

        mp_printf(print, ", (duty=%.2f%%, resolution=%.3f%%)", 100.0 * get_duty_raw(self) / (1 << resolution), 100.0 * 1 / (1 << resolution)); // percents

        mp_printf(print, ", mode=%d, channel=%d, timer=%d", self->mode, self->channel, self->timer);

        int clk_src = timers[TIMER_IDX(self->mode, self->timer)].clk_cfg;
        if(clk_src == LEDC_USE_APB_CLK) {
            mp_printf(print, ", clock=PWM_APB_CLK(%d)",PWM_APB_CLK);
        } else if (clk_src == LEDC_USE_RC_FAST_CLK) {
            mp_printf(print, ", clock=PWM_RC_FAST_CLK(%d)",PWM_RC_FAST_CLK);
        } 
        #if SOC_LEDC_SUPPORT_XTAL_CLOCK
        else if (clk_src == LEDC_USE_XTAL_CLK) {
            mp_printf(print, ", clock=PWM_XTAL_CLK(%d)",PWM_XTAL_CLK);
        }
        #endif
        #if SOC_LEDC_SUPPORT_REF_TICK
        else if (clk_src == LEDC_USE_REF_TICK) {
            mp_printf(print, ", clock=PWM_REF_TICK(%d)",PWM_REF_TICK);
        } 
        #endif
        else {
             mp_printf(print, ", clock=UNKNOWN");
        }
        
        if(self->lightsleepenabled) {
            mp_printf(print, ", light sleep enabled");
        }
    }
    mp_printf(print, ")");
}

// This called from pwm.init() method
static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty, ARG_duty_u16, ARG_duty_ns, ARG_clock, ARG_lightSleepEnable };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_clock, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PWM_AUTO_CLK} },
        { MP_QSTR_light_sleep_enable, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int freq = args[ARG_freq].u_int;

    // ***************************** Decode clok and light sleep mode *********************************
    int pwm_src_clock = args[ARG_clock].u_int;
    if (pwm_src_clock >= _PWM_LAST_CLK_IDX) {
        mp_raise_ValueError(MP_ERROR_TEXT("Bad value for clock."));
    }

    // Check if the clock is available:
    if (clk_source_map[pwm_src_clock] < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Clock source not available for this Soc."));
    }

    if(args[ARG_lightSleepEnable].u_int > 0) {
        // The light sleep enabled is requested 
        // => GPIO need to be in the not disabled list

        if (pwm_src_clock == PWM_AUTO_CLK) {
            // In case of Clock auto select, with sleep mode the clock will be PWM_RC_FAST_CLK
            pwm_src_clock = PWM_RC_FAST_CLK;        
        }
        // Check if the source clock is valid for light sleep (only LEDC_USE_RC_FAST_CLK is valid )
        else if (pwm_src_clock != PWM_RC_FAST_CLK)  {
            mp_raise_ValueError(MP_ERROR_TEXT("only PWM_RC_FAST_CLK is allowed with light sleep enabled"));
        }
        self->lightsleepenabled = true;
        
    } else {
        self->lightsleepenabled = false;
    }

    // if auto clock => Determine the best clock
    if (pwm_src_clock == PWM_AUTO_CLK) {

        #if !(PWM_SUPPORT_INDEP_CLOCK_SRC)
            int pwm_clk = find_clock_in_use();
            if (pwm_clk != PWM_AUTO_CLK) {
                pwm_src_clock = pwm_clk;
            }
            else {
                pwm_src_clock = PWM_APB_CLK;
            }
        #else
            pwm_src_clock = PWM_APB_CLK;

            #if SOC_LEDC_SUPPORT_REF_TICK
            if (freq < EMPIRIC_FREQ) {
                pwm_src_clock = PWM_REF_TICK; // 1 MHz
            }
            #endif
        #endif
    }
    #if !(PWM_SUPPORT_INDEP_CLOCK_SRC)
    else {
        int pwm_clk = find_clock_in_use();
        if ((pwm_clk != PWM_AUTO_CLK) && (pwm_clk != pwm_src_clock)) { 
            mp_raise_ValueError(MP_ERROR_TEXT("one or more active timers use a different clock source, not supported by the current SoC."));
        }
    }
    #endif


    // Note: High Speed Mode (available on ESP32 only, not on S2/S3), only supports REF_TICK(1MHz) and APB_CLK(80MHz)    
    //       Low Speed Mode  (avail. on ESP32, S2, S3, C3, C6) supports [REF_TICK(1MHz)], RC_FAST_CLK(8Mhz) and APB_CLK(80MHz)

    int channel_idx =  (self->lightsleepenabled) ? find_channel(self->pin, 0) : find_channel(self->pin, ANY_MODE);
    
    if (channel_idx == -1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of PWM channels:%d"), PWM_CHANNEL_MAX); // in all modes
    }

    int duty = args[ARG_duty].u_int;
    int duty_u16 = args[ARG_duty_u16].u_int;
    int duty_ns = args[ARG_duty_ns].u_int;
    if (((duty != -1) && (duty_u16 != -1)) || ((duty != -1) && (duty_ns != -1)) || ((duty_u16 != -1) && (duty_ns != -1))) {
        mp_raise_ValueError(MP_ERROR_TEXT("only one of parameters 'duty', 'duty_u16' or 'duty_ns' is allowed"));
    }
    
    // Check if freq wasn't passed as an argument
    if (freq == -1) {
        // Check if already set, otherwise use the default freq.
        // It is possible in case:
        // pwm = PWM(pin, freq=1000, duty=256)
        // pwm = PWM(pin, duty=128)
        if (chans[channel_idx].timer_idx != -1) {
            freq = timers[chans[channel_idx].timer_idx].freq_hz;
        }
        if (freq <= 0) {
            freq = PWM_FREQ;
        }
    }

    if(self->lightsleepenabled) {
        if ((freq <= 0) || (freq > 8000000)) {
            mp_raise_ValueError(MP_ERROR_TEXT("frequency must be from 1Hz to 8MHz"));
        }
    }
    else
        if ((freq <= 0) || (freq > 40000000)) {
            mp_raise_ValueError(MP_ERROR_TEXT("frequency must be from 1Hz to 40MHz"));
        }




    int timer_idx;
    int current_timer_idx = chans[channel_idx].timer_idx;
    bool current_in_use = is_timer_in_use(channel_idx, current_timer_idx);
    if (current_in_use) {
        timer_idx = find_timer(freq, SAME_FREQ_OR_FREE, CHANNEL_IDX_TO_MODE(channel_idx), clk_source_map[pwm_src_clock]);
    } else {
        timer_idx = chans[channel_idx].timer_idx;
    }

    if (timer_idx == -1) {
        if (self->lightsleepenabled) {
            timer_idx = find_timer(freq, SAME_FREQ_OR_FREE, 0, clk_source_map[pwm_src_clock]);
        }
        else {
            timer_idx = find_timer(freq, SAME_FREQ_OR_FREE, ANY_MODE, clk_source_map[pwm_src_clock]);
        }
    }
    if (timer_idx == -1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of PWM timers:%d"), PWM_TIMER_MAX); // in all modes
    }

    #if !(PWM_SUPPORT_INDEP_CLOCK_SRC)
    // Check for the clock source consistency in case of ESP32-S3/C3 and C6
    if( is_timer_with_different_clock(timer_idx, clk_source_map[pwm_src_clock])) {        
        mp_raise_ValueError(MP_ERROR_TEXT("one or more active timers use a different clock source, which is not supported by the current SoC."));
    }
    #endif

    int mode = TIMER_IDX_TO_MODE(timer_idx);
    if (CHANNEL_IDX_TO_MODE(channel_idx) != mode) {
        // unregister old channel
        chans[channel_idx].pin = -1;
        chans[channel_idx].timer_idx = -1;
        chans[channel_idx].lightsleepenabled = false;
        // find new channel
        channel_idx = find_channel(self->pin, mode);
        if (CHANNEL_IDX_TO_MODE(channel_idx) != mode) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of PWM channels:%d"), PWM_CHANNEL_MAX); // in current mode
        }
    }
    self->mode = mode;
    self->timer = TIMER_IDX_TO_TIMER(timer_idx);
    self->channel = CHANNEL_IDX_TO_CHANNEL(channel_idx);

    // New PWM assignment
    if ((chans[channel_idx].pin == -1) || (chans[channel_idx].timer_idx != timer_idx)) {
        configure_channel(self);
        chans[channel_idx].pin = self->pin;
    }
    chans[channel_idx].timer_idx = timer_idx;
    chans[channel_idx].lightsleepenabled = self->lightsleepenabled;
    self->active = true;

    // Set timer frequency
    set_freq(self, freq, &timers[timer_idx], clk_source_map[pwm_src_clock]);

    // Set duty cycle?
    if (duty_u16 != -1) {
        set_duty_u16(self, duty_u16);
    } else if (duty_ns != -1) {
        set_duty_ns(self, duty_ns);
    } else if (duty != -1) {
        set_duty_u10(self, duty);
    } else if (self->duty_x == 0) {
        set_duty_u10(self, (1 << PWM_RES_10_BIT) / 2); // 50%
    }
}

// This called from PWM() constructor
static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 4, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);

    // create PWM object from the given pin
    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->pin = pin_id;
    self->active = false;
    self->lightsleepenabled = false;
    self->mode = -1;
    self->channel = -1;
    self->timer = -1;
    self->duty_x = 0;

    // start the PWM subsystem if it's not already running
    if (!pwm_inited) {
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
    int channel_idx = CHANNEL_IDX(self->mode, self->channel);
    pwm_deinit(channel_idx);    
    self->active = false;
    self->lightsleepenabled = false;
    self->mode = -1;
    self->channel = -1;
    self->timer = -1;
    self->duty_x = 0;
}

// Set and get methods of PWM class

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    pwm_is_active(self);
    return MP_OBJ_NEW_SMALL_INT(ledc_get_freq(self->mode, self->timer));
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    pwm_is_active(self);

    if(self->lightsleepenabled) {
        if ((freq <= 0) || (freq > 8000000)) {
            mp_raise_ValueError(MP_ERROR_TEXT("frequency must be from 1Hz to 8MHz"));
        }
    }
    else
        if ((freq <= 0) || (freq > 40000000)) {
            mp_raise_ValueError(MP_ERROR_TEXT("frequency must be from 1Hz to 40MHz"));
        }
        
    if (freq == timers[TIMER_IDX(self->mode, self->timer)].freq_hz) {
        return;
    }

    
    int current_timer_idx = chans[CHANNEL_IDX(self->mode, self->channel)].timer_idx;
    int led_clk_src = timers[TIMER_IDX(self->mode, current_timer_idx)].clk_cfg;

    bool current_in_use = is_timer_in_use(CHANNEL_IDX(self->mode, self->channel), current_timer_idx);

    // Check if an already running timer with the same freq and clock is running
    int new_timer_idx = find_timer(freq, SAME_FREQ_ONLY, self->mode, led_clk_src);

    // If no existing timer was found, and the current one is in use, then find a new one
    if ((new_timer_idx == -1) && current_in_use) {
        // Have to find a new timer
        new_timer_idx = find_timer(freq, SAME_FREQ_OR_FREE, self->mode, led_clk_src);

        if (new_timer_idx == -1) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of PWM timers:%d"), PWM_TIMER_MAX); // in current mode
        }
    }

    if ((new_timer_idx != -1) && (new_timer_idx != current_timer_idx)) {
        // Bind the channel to the new timer
        chans[self->channel].timer_idx = new_timer_idx;

        if (ledc_bind_channel_timer(self->mode, self->channel, TIMER_IDX_TO_TIMER(new_timer_idx)) != ESP_OK) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("failed to bind timer to channel"));
        }

        if (!current_in_use) {
            // Free the old timer
            check_esp_err(ledc_timer_rst(self->mode, self->timer));
            // Flag it unused
            timers[current_timer_idx].freq_hz = -1;
        }

        current_timer_idx = new_timer_idx;
    }
    self->mode = TIMER_IDX_TO_MODE(current_timer_idx);
    self->timer = TIMER_IDX_TO_TIMER(current_timer_idx);

    // Set the frequency
    set_freq(self, freq, &timers[current_timer_idx], led_clk_src);
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
