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
#include <stdint.h>

#include "esppwm.h"

#include "py/nlr.h"
#include "py/runtime.h"
#include "modpyb.h"

typedef struct _pyb_pwm_obj_t {
    mp_obj_base_t base;
    pyb_pin_obj_t *pin;
    uint8_t active;
    uint8_t channel;
} pyb_pwm_obj_t;

STATIC bool pwm_inited = false;

/******************************************************************************/
// MicroPython bindings for PWM

STATIC void pyb_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(%u", self->pin->phys_port);
    if (self->active) {
        mp_printf(print, ", freq=%u, duty=%u",
            pwm_get_freq(self->channel), pwm_get_duty(self->channel));
    }
    mp_printf(print, ")");
}

STATIC void pyb_pwm_init_helper(pyb_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty, MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int channel = pwm_add(self->pin->phys_port, self->pin->periph, self->pin->func);
    if (channel == -1) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "PWM not supported on pin %d", self->pin->phys_port));
    }

    self->channel = channel;
    self->active = 1;
    if (args[ARG_freq].u_int != -1) {
        pwm_set_freq(args[ARG_freq].u_int, self->channel);
    }
    if (args[ARG_duty].u_int != -1) {
        pwm_set_duty(args[ARG_duty].u_int, self->channel);
    }

    pwm_start();
}

STATIC mp_obj_t pyb_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    pyb_pin_obj_t *pin = mp_obj_get_pin_obj(args[0]);

    // create PWM object from the given pin
    pyb_pwm_obj_t *self = m_new_obj(pyb_pwm_obj_t);
    self->base.type = &pyb_pwm_type;
    self->pin = pin;
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
    pyb_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_pwm_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_pwm_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_pwm_init_obj, 1, pyb_pwm_init);

STATIC mp_obj_t pyb_pwm_deinit(mp_obj_t self_in) {
    pyb_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    pwm_delete(self->channel);
    self->active = 0;
    pwm_start();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pwm_deinit_obj, pyb_pwm_deinit);

STATIC mp_obj_t pyb_pwm_freq(size_t n_args, const mp_obj_t *args) {
    //pyb_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(pwm_get_freq(0));
    } else {
        // set
        pwm_set_freq(mp_obj_get_int(args[1]), 0);
        pwm_start();
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pwm_freq_obj, 1, 2, pyb_pwm_freq);

STATIC mp_obj_t pyb_pwm_duty(size_t n_args, const mp_obj_t *args) {
    pyb_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (!self->active) {
        pwm_add(self->pin->phys_port, self->pin->periph, self->pin->func);
        self->active = 1;
    }
    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(pwm_get_duty(self->channel));
    } else {
        // set
        pwm_set_duty(mp_obj_get_int(args[1]), self->channel);
        pwm_start();
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pwm_duty_obj, 1, 2, pyb_pwm_duty);

STATIC const mp_rom_map_elem_t pyb_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_pwm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&pyb_pwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&pyb_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&pyb_pwm_duty_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_pwm_locals_dict, pyb_pwm_locals_dict_table);

const mp_obj_type_t pyb_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = pyb_pwm_print,
    .make_new = pyb_pwm_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_pwm_locals_dict,
};
