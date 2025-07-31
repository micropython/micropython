/*
 * This file is part of the MicroPython project, http://micropython.org/
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_pwm.c via MICROPY_PY_MACHINE_PWM_INCLUDEFILE.

#include "py/mphal.h"
#include "esppwm.h"

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    pyb_pin_obj_t *pin;
    uint8_t active;
    uint8_t channel;
    int32_t duty_ns;
} machine_pwm_obj_t;

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty);

static bool pwm_inited = false;

/******************************************************************************/
// MicroPython bindings for PWM

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(%u", self->pin->phys_port);
    if (self->active) {
        mp_printf(print, ", freq=%u, duty=%u",
            pwm_get_freq(self->channel), pwm_get_duty(self->channel));
    }
    mp_printf(print, ")");
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty, ARG_duty_u16, ARG_duty_ns };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns, MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int channel = pwm_add(self->pin->phys_port, self->pin->periph, self->pin->func);
    if (channel == -1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PWM not supported on pin %d"), self->pin->phys_port);
    }

    self->channel = channel;
    self->active = 1;
    if (args[ARG_freq].u_int != -1) {
        pwm_set_freq(args[ARG_freq].u_int, self->channel);
    }
    if (args[ARG_duty].u_int != -1) {
        pwm_set_duty(args[ARG_duty].u_int, self->channel);
    }
    if (args[ARG_duty_u16].u_int != -1) {
        pwm_set_duty(args[ARG_duty_u16].u_int * 1000 / 65535, self->channel);
    }
    if (args[ARG_duty_ns].u_int != -1) {
        uint32_t freq = pwm_get_freq(0);
        if (freq > 0) {
            pwm_set_duty((uint64_t)args[ARG_duty_ns].u_int * freq / 1000000, self->channel);
        }
    }

    if (pin_mode[self->pin->phys_port] == GPIO_MODE_OPEN_DRAIN) {
        mp_hal_pin_open_drain(self->pin->phys_port);
    }

    pwm_start();
}

static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    pyb_pin_obj_t *pin = mp_obj_get_pin_obj(args[0]);

    // create PWM object from the given pin
    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->pin = pin;
    self->active = 0;
    self->channel = -1;
    self->duty_ns = -1;

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

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    pwm_delete(self->channel);
    self->active = 0;
    pwm_start();
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(pwm_get_freq(0));
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    pwm_set_freq(freq, 0);
    if (self->duty_ns != -1) {
        mp_machine_pwm_duty_set_ns(self, self->duty_ns);
    } else {
        pwm_start();
    }
}

static void set_active(machine_pwm_obj_t *self, bool set_pin) {
    if (!self->active) {
        pwm_add(self->pin->phys_port, self->pin->periph, self->pin->func);
        self->active = 1;
        if (set_pin && pin_mode[self->pin->phys_port] == GPIO_MODE_OPEN_DRAIN) {
            mp_hal_pin_open_drain(self->pin->phys_port);
        }
    }
}

static mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self) {
    set_active(self, true);
    return MP_OBJ_NEW_SMALL_INT(pwm_get_duty(self->channel));
}

static void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty) {
    set_active(self, false);
    self->duty_ns = -1;
    pwm_set_duty(duty, self->channel);
    pwm_start();
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    set_active(self, true);
    return MP_OBJ_NEW_SMALL_INT(pwm_get_duty(self->channel) * 65535 / 1024);
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty) {
    set_active(self, false);
    self->duty_ns = -1;
    pwm_set_duty(duty * 1024 / 65535, self->channel);
    pwm_start();
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    set_active(self, true);
    uint32_t freq = pwm_get_freq(0);
    return MP_OBJ_NEW_SMALL_INT(pwm_get_duty(self->channel) * 976563 / freq);
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty) {
    set_active(self, false);
    self->duty_ns = duty;
    uint32_t freq = pwm_get_freq(0);
    pwm_set_duty(duty * freq / 976562, self->channel); // 1e9/1024 = 976562.5
    pwm_start();
}
