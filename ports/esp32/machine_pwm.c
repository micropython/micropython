/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#include "py/nlr.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"

// Forward dec'l
extern const mp_obj_type_t machine_pwm_type;


typedef struct _esp32_pwm_obj_t {
    mp_obj_base_t base;
    gpio_num_t pin;
    uint8_t active;
    uint8_t channel;
    uint8_t timer;
    uint8_t mode;
} esp32_pwm_obj_t;

// Number of available timers
#define LEDC_TIMER_MAX 4

// Which channel has which GPIO pin assigned?
// (-1 if not assigned)
STATIC int chan_gpio[LEDC_CHANNEL_MAX];

// Params for PW operation
// 5khz
#define PWFREQ (5000)
// 10-bit resolution (compatible with esp8266 PWM)
#define PWRES (LEDC_TIMER_10_BIT)

// Config of timer upon which we run all PWM'ed GPIO pins
STATIC bool pwm_inited = false;

STATIC void pwm_init(void) {
    // Initial condition: no channels assigned
    for (int x = 0; x < LEDC_CHANNEL_MAX; ++x) {
        chan_gpio[x] = -1;
    }

    // prepare all timers
    ledc_timer_config_t timer_cfg = {
        .bit_num = PWRES,
        .freq_hz = PWFREQ,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = 0,
    };
    for (int mode = 0; mode < LEDC_SPEED_MODE_MAX; ++mode) {
        for (int idx = 0; idx < LEDC_TIMER_MAX; ++idx) {
            timer_cfg.timer_num = idx;
            timer_cfg.speed_mode = mode;
            ledc_timer_config(&timer_cfg);
        }
    }
}

/******************************************************************************/

// MicroPython bindings for PWM

STATIC void esp32_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp32_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(%u, timer=%u, speed_mode=%u", self->pin, self->timer, self->mode);
    if (self->active) {
        mp_printf(print, ", freq=%u, duty=%u",
            ledc_get_freq(self->mode, self->timer),
            ledc_get_duty(self->mode, self->channel));
    }
    mp_printf(print, ")");
}

STATIC void esp32_pwm_init_helper(esp32_pwm_obj_t *self,
        size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty, ARG_timer, ARG_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timer, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_speed_mode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = LEDC_HIGH_SPEED_MODE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int channel;
    int avail = -1;

    // Find a free PWM channel, also spot if our pin is
    //  already mentioned.
    for (channel = 0; channel < LEDC_CHANNEL_MAX; ++channel) {
        if (chan_gpio[channel] == self->pin) {
            break;
        }
        if ((avail == -1) && (chan_gpio[channel] == -1)) {
            avail = channel;
        }
    }
    if (channel >= LEDC_CHANNEL_MAX) {
        if (avail == -1) {
            mp_raise_ValueError("out of PWM channels");
        }
        channel = avail;
    }
    self->channel = channel;

    // set timer
    if (0 <= args[ARG_timer].u_int && args[ARG_timer].u_int <= 3) {
        self->timer = args[ARG_timer].u_int;
    } else {
        mp_raise_ValueError("invalid timer");
    }

    // set speed mode
    if (0 <= args[ARG_mode].u_int && args[ARG_mode].u_int <= LEDC_SPEED_MODE_MAX) {
        self->mode = args[ARG_mode].u_int;
    } else {
        mp_raise_ValueError("invalid speed_mode");
    }

    // New PWM assignment
    self->active = 1;
    if (chan_gpio[channel] == -1) {
        // configure channel
        ledc_channel_config_t cfg = {
            .channel = channel,
            .duty = (1 << PWRES) / 2,
            .gpio_num = self->pin,
            .intr_type = LEDC_INTR_DISABLE,
            .speed_mode = self->mode,
            .timer_sel = self->timer,
        };
        if (ledc_channel_config(&cfg) != ESP_OK) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                "PWM not supported on pin %d", self->pin));
        }
        chan_gpio[channel] = self->pin;
    }

    // Maybe change PWM timer
    int fval = args[ARG_freq].u_int;
    if (fval != -1 && ledc_set_freq(self->mode, self->timer, fval) != ESP_OK) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "Bad frequency %d", fval));
    }

    // Set duty cycle?
    int dval = args[ARG_duty].u_int;
    if (dval != -1) {
        dval &= ((1 << PWRES)-1);
        ledc_set_duty(self->mode, channel, dval);
        ledc_update_duty(self->mode, channel);
    }

    // Reset the timer if low speed
    if (self->mode == LEDC_LOW_SPEED_MODE) {
        ledc_timer_rst(self->mode, self->timer);
    }
}

STATIC mp_obj_t esp32_pwm_make_new(const mp_obj_type_t *type,
        size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    gpio_num_t pin_id = machine_pin_get_id(args[0]);

    // create PWM object from the given pin
    esp32_pwm_obj_t *self = m_new_obj(esp32_pwm_obj_t);
    self->base.type = &machine_pwm_type;
    self->pin = pin_id;
    self->active = 0;
    self->channel = -1;

    // start the PWM subsystem if it's not already running
    if (!pwm_inited) {
        pwm_init();
        pwm_inited = true;
    }

    // start the PWM running for this channel
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    esp32_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t esp32_pwm_init(size_t n_args,
        const mp_obj_t *args, mp_map_t *kw_args) {
    esp32_pwm_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(esp32_pwm_init_obj, 1, esp32_pwm_init);

STATIC mp_obj_t esp32_pwm_deinit(mp_obj_t self_in) {
    esp32_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int chan = self->channel;

    // Valid channel?
    if ((chan >= 0) && (chan < LEDC_CHANNEL_MAX)) {
        // Mark it unused, and tell the hardware to stop
        chan_gpio[chan] = -1;
        ledc_stop(self->mode, chan, 0);
        self->active = 0;
        self->channel = -1;
        // Disable ledc signal for the pin
        if (self->mode == LEDC_LOW_SPEED_MODE) {
            gpio_matrix_out(self->pin, LEDC_LS_SIG_OUT0_IDX + self->channel, false, true);
        } else {
            gpio_matrix_out(self->pin, LEDC_HS_SIG_OUT0_IDX + self->channel, false, true);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp32_pwm_deinit_obj, esp32_pwm_deinit);

STATIC mp_obj_t esp32_pwm_freq(size_t n_args, const mp_obj_t *args) {
    esp32_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(ledc_get_freq(self->mode, self->timer));
    }

    // set
    int tval = mp_obj_get_int(args[1]);
    if (tval <= 0 || ledc_set_freq(self->mode, self->timer, tval) != ESP_OK) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "bad frequency %d", tval));
    }

    // Reset the timer if low speed
    if (self->mode == LEDC_LOW_SPEED_MODE) {
        ledc_timer_rst(self->mode, self->timer);
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_pwm_freq_obj, 1, 2, esp32_pwm_freq);

STATIC mp_obj_t esp32_pwm_duty(size_t n_args, const mp_obj_t *args) {
    esp32_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int duty;

    if (n_args == 1) {
        // get
        duty = ledc_get_duty(self->mode, self->channel);
        return MP_OBJ_NEW_SMALL_INT(duty);
    }

    // set
    duty = mp_obj_get_int(args[1]);
    duty &= ((1 << PWRES)-1);
    ledc_set_duty(self->mode, self->channel, duty);
    ledc_update_duty(self->mode, self->channel);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_pwm_duty_obj,
    1, 2, esp32_pwm_duty);

STATIC const mp_rom_map_elem_t esp32_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&esp32_pwm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&esp32_pwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&esp32_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&esp32_pwm_duty_obj) },
    { MP_ROM_QSTR(MP_QSTR_HIGH_SPEED_MODE), MP_ROM_INT(LEDC_HIGH_SPEED_MODE) },
    { MP_ROM_QSTR(MP_QSTR_LOW_SPEED_MODE), MP_ROM_INT(LEDC_LOW_SPEED_MODE) },
};

STATIC MP_DEFINE_CONST_DICT(esp32_pwm_locals_dict,
    esp32_pwm_locals_dict_table);

const mp_obj_type_t machine_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = esp32_pwm_print,
    .make_new = esp32_pwm_make_new,
    .locals_dict = (mp_obj_dict_t*)&esp32_pwm_locals_dict,
};
