/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <math.h>

#include "py/runtime.h"
#include "py/mphal.h"

#include "driver/ledc.h"
#include "esp_err.h"

// #define DBG(...)
#define DBG(...) mp_printf(&mp_plat_print, __VA_ARGS__)

// Total number of channels
#define PWM_CHANNEL_MAX (LEDC_SPEED_MODE_MAX * LEDC_CHANNEL_MAX)
typedef struct _chan_t {
    // Which channel has which GPIO pin assigned?
    // (-1 if not assigned)
    gpio_num_t pin;
    // Which channel has which timer assigned?
    // (-1 if not assigned)
    int timer_idx;
} chan_t;
// List of PWM channels
STATIC chan_t chans[PWM_CHANNEL_MAX];

// channel_idx is an index (end-to-end sequential numbering) for all channels
// available on the chip and described in chans[]
#define CHANNEL_IDX(mode, channel) (mode * LEDC_CHANNEL_MAX + channel)
#define CHANNEL_IDX_TO_MODE(channel_idx) (channel_idx / LEDC_CHANNEL_MAX)
#define CHANNEL_IDX_TO_CHANNEL(channel_idx) (channel_idx % LEDC_CHANNEL_MAX)

// Total number of timers
#define PWM_TIMER_MAX (LEDC_SPEED_MODE_MAX * LEDC_TIMER_MAX)
// List of timer configs
STATIC ledc_timer_config_t timers[PWM_TIMER_MAX];

// timer_idx is an index (end-to-end sequential numbering) for all timers
// available on the chip and configured in timers[]
#define TIMER_IDX(mode, timer) (mode * LEDC_TIMER_MAX + timer)
#define TIMER_IDX_TO_MODE(timer_idx) (timer_idx / LEDC_TIMER_MAX)
#define TIMER_IDX_TO_TIMER(timer_idx) (timer_idx % LEDC_TIMER_MAX)

// Params for PW operation
// 5khz is default frequency
#define PWFREQ (5000)

// 10-bit resolution (compatible with esp8266 PWM)
#define PWRES (LEDC_TIMER_10_BIT)
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html#supported-range-of-frequency-and-duty-resolutions
// duty() uses 10-bit resolution or less
// duty_u16() and duty_ns() use 16-bit resolution or less

// possible highest resolution in device
#if CONFIG_IDF_TARGET_ESP32
#define HIGHEST_PWM_RES (LEDC_TIMER_16_BIT) // 20 in fact, but 16 is used
#else
#define HIGHEST_PWM_RES (LEDC_TIMER_14_BIT)
#endif
// duty resolution of user interface in `duty_u16()` and `duty_u16` parameter in constructor/initializer
#define UI_RES_16_BIT (16)
// how much to shift from the HIGHEST_PWM_RES duty resolution to the user interface duty resolution UI_RES_16_BIT
#define UI_RES_SHIFT (16 - HIGHEST_PWM_RES) // 0 for ESP32, 2 for S2, S3, C3

// If the PWM frequency is less than EMPIRIC_FREQ, then LEDC_REF_CLK_HZ(1 MHz) is used, else LEDC_APB_CLK_HZ(80 MHz) is used
#define EMPIRIC_FREQ (10) // Hz

// Config of timer upon which we run all PWM'ed GPIO pins
STATIC bool pwm_inited = false;

// MicroPython PWM object struct
typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    gpio_num_t pin;
    bool active;
    int mode;
    int channel;
    int timer;
    int duty_x; // PWRES if duty(), HIGHEST_PWM_RES if duty_u16(), -HIGHEST_PWM_RES if duty_ns()
    int duty_u10;
    int duty_u16;
    int duty_ns;
} machine_pwm_obj_t;

STATIC void pwm_init(void) {
    // Initial condition: no channels assigned
    for (int i = 0; i < PWM_CHANNEL_MAX; ++i) {
        chans[i].pin = -1;
        chans[i].timer_idx = -1;
    }

    // Prepare all timers config
    // Initial condition: no timers assigned
    for (int i = 0; i < PWM_TIMER_MAX; ++i) {
        timers[i].duty_resolution = HIGHEST_PWM_RES;
        // unset timer is -1
        timers[i].freq_hz = -1;
        timers[i].speed_mode = TIMER_IDX_TO_MODE(i);
        timers[i].timer_num = TIMER_IDX_TO_TIMER(i);
        timers[i].clk_cfg = LEDC_AUTO_CLK;
    }
}

STATIC void configure_channel(machine_pwm_obj_t *self) {
    ledc_channel_config_t cfg = {
        .channel = self->channel,
        .duty = (1 << (timers[TIMER_IDX(self->mode, self->timer)].duty_resolution)) / 2,
        .gpio_num = self->pin,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = self->mode,
        .timer_sel = self->timer,
    };
    if (ledc_channel_config(&cfg) != ESP_OK) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM not supported on Pin(%d)"), self->pin);
    }
}

STATIC void set_duty_u16(machine_pwm_obj_t *self, int duty);
STATIC void set_duty_u10(machine_pwm_obj_t *self, int duty);
STATIC void set_duty_ns(machine_pwm_obj_t *self, int ns);

STATIC void set_freq(int freq, ledc_timer_config_t *timer, machine_pwm_obj_t *self) {
    // If already set, do nothing
    if (freq == timer->freq_hz) {
        return;
    }

    // Find the highest bit resolution for the requested frequency
    if (freq <= 0) {
        freq = 1;
    }
    unsigned int res = 0;
    unsigned int i = LEDC_APB_CLK_HZ; // 80 MHz
    if (freq < EMPIRIC_FREQ) {
        i = LEDC_REF_CLK_HZ; // 1 MHz
    }
    //int divider = i / freq; // truncated
    int divider = (i + freq / 2) / freq; // rounded
    float f = i / divider; // actual frequency
    i = round(i / f);
    for (; i > 1; i >>= 1) {
        ++res;
    }
    if (res == 0) {
        res = 1;
    } else if (res > HIGHEST_PWM_RES) {
        // Limit resolution to HIGHEST_PWM_RES to match units of our duty
        res = HIGHEST_PWM_RES;
    }

    // Configure the new resolution and frequency
    timer->duty_resolution = res;
    timer->freq_hz = freq;
    if (freq < EMPIRIC_FREQ) {
        timer->clk_cfg = LEDC_USE_REF_TICK;
    } else {
        timer->clk_cfg = LEDC_USE_APB_CLK;
    }

    // set freq
    esp_err_t err = ledc_timer_config(timer);
    if (err != ESP_OK) {
        if (err == ESP_FAIL) {
            DBG(" (timer timer->speed_mode %d, timer->timer_num %d, timer->clk_cfg %d, timer->freq_hz  %d, timer->duty_resolution %d) ", timer->speed_mode, timer->timer_num, timer->clk_cfg, timer->freq_hz, timer->duty_resolution);
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("unreachable frequency %d"), freq);
        } else {
            check_esp_err(err);
        }
    }

    // Save the same duty cycle when freq is changed
    if (self->duty_x == PWRES) {
        set_duty_u10(self, self->duty_u10);
    } else if (self->duty_x == HIGHEST_PWM_RES ) {
        set_duty_u16(self, self->duty_u16);
    } else if (self->duty_x == -HIGHEST_PWM_RES ) {
        set_duty_ns(self, self->duty_ns);
    }
}

// Calculate the duty parameters based on an ns value
STATIC int ns_to_duty(machine_pwm_obj_t *self, int ns) {
    ledc_timer_config_t timer = timers[TIMER_IDX(self->mode, self->timer)];
    int freq = timer.freq_hz;
    int resolution = 1 << timer.duty_resolution;
    int64_t duty = ((int64_t)ns * resolution * freq + 500000000) / 1000000000;
    if ((ns !=0) && (duty == 0)) {
        duty = 1;
    }
    // DBG(" (ns=%d resolution=%d freq=%d -> duty=%f=%d) ", ns, resolution , freq, 1.0 * ns * resolution * freq / 1000000000.0, duty);
    return duty;
}

STATIC int duty_to_ns(machine_pwm_obj_t *self, int duty) {
    ledc_timer_config_t timer = timers[TIMER_IDX(self->mode, self->timer)];
    int freq = timer.freq_hz;
    int resolution = 1 << timer.duty_resolution;
    int64_t ns = ((int64_t)1000000000 * duty + freq * resolution / 2) / (freq * resolution);
    // DBG(" (resolution=%d freq=%d duty=%d -> ns=%f=%d) ", resolution , freq, duty, 1000000000.0 / freq * duty / resolution, ns);
    return ns;
}

#define get_duty_raw(self) ledc_get_duty(self->mode, self->channel)

STATIC int get_duty_u16(machine_pwm_obj_t *self) {
    uint32_t duty = ledc_get_duty(self->mode, self->channel);
    return duty << (HIGHEST_PWM_RES + UI_RES_SHIFT - timers[TIMER_IDX(self->mode, self->timer)].duty_resolution);
}

STATIC int get_duty_u10(machine_pwm_obj_t *self) {
    return get_duty_u16(self) >> (HIGHEST_PWM_RES - PWRES);
}

STATIC int get_duty_ns(machine_pwm_obj_t *self) {
    return duty_to_ns(self, get_duty_u16(self));
}

STATIC void set_duty_u16(machine_pwm_obj_t *self, int duty) {
    int max_duty = (1 << UI_RES_16_BIT) - 1;
    if ((duty < 0) || (duty > max_duty)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty_u16 must be from 0 to %d"), max_duty);
    }
    duty >>= HIGHEST_PWM_RES + UI_RES_SHIFT - timers[TIMER_IDX(self->mode, self->timer)].duty_resolution;
    check_esp_err(ledc_set_duty(self->mode, self->channel, duty));
    check_esp_err(ledc_update_duty(self->mode, self->channel));
    // check_esp_err(ledc_set_duty_and_update(self->mode, self->channel, duty, (1 << PWRES) - 1)); // thread safe function ???

    // Bug: Sometimes duty is not set right now.
    // See https://github.com/espressif/esp-idf/issues/7288
    /*
    if (duty != get_duty_u16(self)) {
        DBG("\n duty_set_u16 %u %u %d \n", duty, get_duty_u16(self), timers[TIMER_IDX(self->mode, self->timer)].duty_resolution);
    }
    */
    self->duty_x = HIGHEST_PWM_RES;
    self->duty_u16 = duty;
}

STATIC void set_duty_u10(machine_pwm_obj_t *self, int duty) {
    int max_duty = (1 << PWRES) - 1;
    if ((duty < 0) || (duty > max_duty)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty must be from 0 to %d"), max_duty);
    }
    set_duty_u16(self, duty << (HIGHEST_PWM_RES + UI_RES_SHIFT - PWRES));
    self->duty_x = PWRES;
    self->duty_u10 = duty;
}

STATIC void set_duty_ns(machine_pwm_obj_t *self, int ns) {
    int max_duty = (1 << HIGHEST_PWM_RES) - 1;
    if ((ns < 0) || (ns > duty_to_ns(self, max_duty))) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("duty_ns must be from 0 to %d ns"), duty_to_ns(self, max_duty));
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
STATIC int find_timer(int freq, bool same_freq_only, int mode) {
    int free_timer_idx_found = -1;
    // Find a free PWM Timer using the same freq
    for (int timer_idx = 0; timer_idx < PWM_TIMER_MAX; ++timer_idx) {
        if ((mode == ANY_MODE) || (mode == TIMER_IDX_TO_MODE(timer_idx))) {
            if (timers[timer_idx].freq_hz == freq) {
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
STATIC bool is_timer_in_use(int current_channel_idx, int timer_idx) {
    for (int i = 0; i < PWM_CHANNEL_MAX; ++i) {
        if ((i != current_channel_idx) && (chans[i].timer_idx == timer_idx)) {
            return true;
        }
    }

    return false;
}

// Find a free PWM channel, also spot if our pin is already mentioned.
// Return channel_idx. Use CHANNEL_IDX_TO_MODE(channel_idx) and CHANNEL_IDX_TO_CHANNEL(channel_idx) to get mode and channel
STATIC int find_channel(int pin, int mode) {
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

STATIC void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(pin=%u", self->pin);
    if (self->active) {
        mp_printf(print, ", freq=%u", ledc_get_freq(self->mode, self->timer));

        int duty;
        int resolution = timers[TIMER_IDX(self->mode, self->timer)].duty_resolution;
        if (self->duty_x == PWRES) {
            duty = get_duty_u10(self);
            mp_printf(print, ", duty=%d, resolution=%d", duty, resolution);
        } else if (self->duty_x == -HIGHEST_PWM_RES) {
            duty = get_duty_ns(self);
            mp_printf(print, ", duty_ns=%d, resolution=%d", duty, resolution);
        } else {
            duty = get_duty_u16(self);
            mp_printf(print, ", duty_u16=%d, resolution=%d", duty, resolution);
        }
        duty = get_duty_raw(self);
        mp_printf(print, ", duty=%.2f%%, resolution=%.3f%%", 100.0 * duty / (1 << resolution), 100.0 * 1 / (1 << resolution)); // percents

        //mp_printf(print, ", mode=%d, channel=%d, timer=%d", self->mode, self->channel, self->timer);
    }
    mp_printf(print, ")");
}

STATIC void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty, ARG_duty_u16, ARG_duty_ns };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int channel_idx = find_channel(self->pin, ANY_MODE);
    if (channel_idx == -1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of PWM channels:%d"), PWM_CHANNEL_MAX); // in all modes
    }

    int duty = args[ARG_duty].u_int;
    int duty_u16 = args[ARG_duty_u16].u_int;
    int duty_ns = args[ARG_duty_ns].u_int;
    if (((duty != -1) && (duty_u16 != -1)) || ((duty != -1) && (duty_ns != -1))  || ((duty_u16 != -1) && (duty_ns != -1))) {
        mp_raise_ValueError(MP_ERROR_TEXT("only one of parameters 'duty', 'duty_u16' or 'duty_ns' is allowed"));
    }

    int freq = args[ARG_freq].u_int;
    if ((freq < -1) || (freq > 40000000)) {
        mp_raise_ValueError(MP_ERROR_TEXT("freqency must be from 1Hz to 40MHz"));
    }
    // Check if freq wasn't passed as an argument
    if (freq == -1) {
        // Check if already set, otherwise use the default freq.
        // Possible case:
        // pwm = PWM(pin, freq=1000, duty=256)
        // pwm = PWM(pin, duty=128)
        if (chans[channel_idx].timer_idx != -1) {
            freq = timers[chans[channel_idx].timer_idx].freq_hz;
        }
        if (freq < 0) {
            freq = PWFREQ;
        }
    }

    int timer_idx = find_timer(freq, SAME_FREQ_OR_FREE, CHANNEL_IDX_TO_MODE(channel_idx));
    if (timer_idx == -1) {
        timer_idx = find_timer(freq, SAME_FREQ_OR_FREE, ANY_MODE);
    }
    if (timer_idx == -1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("out of PWM timers:%d"), PWM_TIMER_MAX); // in all modes
    }

    int mode = TIMER_IDX_TO_MODE(timer_idx);
    if (CHANNEL_IDX_TO_MODE(channel_idx) != mode) {
        // unregister old channel
        chans[channel_idx].pin = -1;
        chans[channel_idx].timer_idx = -1;
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
    self->active = true;

    // Set timer frequency
    set_freq(freq, &timers[timer_idx], self);

    // Set duty cycle?
    if (duty != -1) {
        set_duty_u10(self, duty);
    } else if (duty_u16 != -1) {
        set_duty_u16(self, duty_u16);
    } else if (duty_ns != -1) {
        set_duty_ns(self, duty_ns);
    } else if (self->duty_x == 0) {
        set_duty_u10(self, (1 << PWRES) / 2);
    }

    // Reset the timer if low speed
    if (self->mode == LEDC_LOW_SPEED_MODE) {
        check_esp_err(ledc_timer_rst(self->mode, self->timer));
    }
}

STATIC mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);

    // create PWM object from the given pin
    machine_pwm_obj_t *self = m_new_obj(machine_pwm_obj_t);
    self->base.type = &machine_pwm_type;
    self->pin = pin_id;
    self->active = false;
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

STATIC void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    int chan = CHANNEL_IDX(self->mode, self->channel);

    // Valid channel?
    if ((chan >= 0) && (chan < PWM_CHANNEL_MAX)) {
        // Clean up timer if necessary
        if (!is_timer_in_use(chan, chans[chan].timer_idx)) {
            check_esp_err(ledc_timer_rst(self->mode, self->timer));
            // Flag it unused
            timers[chans[chan].timer_idx].freq_hz = -1;
        }

        // Mark it unused, and tell the hardware to stop routing
        check_esp_err(ledc_stop(self->mode, chan, 0));
        // Disable ledc signal for the pin
        // gpio_matrix_out(self->pin, SIG_GPIO_OUT_IDX, false, false);
        if (self->mode == LEDC_LOW_SPEED_MODE) {
            gpio_matrix_out(self->pin, LEDC_LS_SIG_OUT0_IDX + self->channel, false, true);
        } else {
            #if LEDC_SPEED_MODE_MAX > 1
            #if CONFIG_IDF_TARGET_ESP32
            gpio_matrix_out(self->pin, LEDC_HS_SIG_OUT0_IDX + self->channel, false, true);
            #else
            #error Add supported CONFIG_IDF_TARGET_ESP32_xxx
            #endif
            #endif
        }
        chans[chan].pin = -1;
        chans[chan].timer_idx = -1;
        self->active = false;
        self->mode = -1;
        self->channel = -1;
        self->timer = -1;
        self->duty_x = 0;
    }
}

STATIC mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(ledc_get_freq(self->mode, self->timer));
}

STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    if (freq == timers[TIMER_IDX(self->mode, self->timer)].freq_hz) {
        return;
    }

    int current_timer_idx = chans[CHANNEL_IDX(self->mode, self->channel)].timer_idx;
    bool current_in_use = is_timer_in_use(CHANNEL_IDX(self->mode, self->channel), current_timer_idx);

    // Check if an already running timer with the same freq is running
    int new_timer_idx = find_timer(freq, SAME_FREQ_ONLY, self->mode);

    // If no existing timer was found, and the current one is in use, then find a new one
    if ((new_timer_idx == -1) && current_in_use) {
        // Have to find a new timer
        new_timer_idx = find_timer(freq, SAME_FREQ_OR_FREE, self->mode);

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

    // Set the freq
    set_freq(freq, &timers[current_timer_idx], self);

    // Reset the timer if low speed
    if (self->mode == LEDC_LOW_SPEED_MODE) {
        check_esp_err(ledc_timer_rst(self->mode, self->timer));
    }
}

STATIC mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(get_duty_u10(self));
}

STATIC void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty) {
    set_duty_u10(self, duty);
}

STATIC mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(get_duty_u16(self));
}

STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    set_duty_u16(self, duty_u16);
}

STATIC mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(get_duty_ns(self));
}

STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    set_duty_ns(self, duty_ns);
}
