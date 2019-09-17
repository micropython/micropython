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

#include "wm_include.h"
#include "wm_gpio_afsel.h"

#include "py/nlr.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"

extern const mp_obj_type_t machine_pwm_type;

typedef struct _w600_pwm_obj_t {
    mp_obj_base_t base;
    mp_hal_pin_obj_t pin;
    uint8_t channel;
    uint8_t duty;
    uint8_t pnum;
    uint32_t freq;
} w600_pwm_obj_t;

/******************************************************************************/

// MicroPython bindings for PWM

STATIC void w600_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    w600_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(%u, freq=%u, duty=%u, pnum=%u, pin=%u)", self->channel,
              self->freq, self->duty, self->pnum, self->pin);
}

STATIC int w600_pwm_multiplex_config(w600_pwm_obj_t *self) {
    switch (self->channel) {
    case 0:
        wm_pwm1_config(self->pin);
    case 1:
        wm_pwm2_config(self->pin);
    case 2:
        wm_pwm3_config(self->pin);
    case 3:
        wm_pwm4_config(self->pin);
    case 4:
        wm_pwm5_config(self->pin);
    default:
        return -1;
    }
}

STATIC void w600_pwm_init_helper(w600_pwm_obj_t *self,
                                 size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_channel, ARG_freq, ARG_duty, ARG_pnum };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_channel,  MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_duty, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_pnum, MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int tval = args[ARG_channel].u_int;
    if ((tval < 0) || (tval > 4)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad channel %d", tval));
    }
    self->channel = tval;

    tval = args[ARG_freq].u_int;
    if ((tval < 1) || (tval > 156250)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad frequency %d", tval));
    }
    self->freq = tval;

    tval = args[ARG_duty].u_int;
    if ((tval < 0) || (tval > 255)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad duty %d", tval));
    }
    self->duty = tval;

    tval = args[ARG_pnum].u_int;
    if ((tval < 0) || (tval > 255)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad period num %d", tval));
    }
    self->pnum = tval;

    w600_pwm_multiplex_config(self);
    tls_pwm_init(self->channel, self->freq, self->duty, self->pnum);
    tls_pwm_start(self->channel);
}

STATIC mp_obj_t w600_pwm_make_new(const mp_obj_type_t *type,
                                  size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // create PWM object from the given pin
    w600_pwm_obj_t *self = m_new_obj(w600_pwm_obj_t);
    self->base.type = &machine_pwm_type;
    self->pin = mp_hal_get_pin_obj(args[0]);
    self->channel = 0;
    self->freq = 1;
    self->duty = 0;
    self->pnum = 0;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    w600_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t w600_pwm_init(size_t n_args,
                              const mp_obj_t *args, mp_map_t *kw_args) {
    w600_pwm_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(w600_pwm_init_obj, 1, w600_pwm_init);

STATIC mp_obj_t w600_pwm_deinit(mp_obj_t self_in) {
    w600_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    tls_pwm_stop(self->channel);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(w600_pwm_deinit_obj, w600_pwm_deinit);

STATIC mp_obj_t w600_pwm_freq(size_t n_args, const mp_obj_t *args) {
    w600_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        return MP_OBJ_NEW_SMALL_INT(self->freq);
    }

    // set
    int tval = mp_obj_get_int(args[1]);
    if ((tval < 1) || (tval > 156250)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad frequency %d", tval));
    }
    tls_pwm_freq_set(self->channel, tval);
    self->freq = tval;
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(w600_pwm_freq_obj, 1, 2, w600_pwm_freq);

STATIC mp_obj_t w600_pwm_duty(size_t n_args, const mp_obj_t *args) {
    w600_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int duty;

    if (n_args == 1) {
        // get
        duty = self->duty;
        return MP_OBJ_NEW_SMALL_INT(duty);
    }

    // set
    duty = mp_obj_get_int(args[1]);
    if ((duty < 0) || (duty > 255)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Bad duty %d", duty));
    }
    tls_pwm_duty_set(self->channel, duty);
    self->duty = duty;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(w600_pwm_duty_obj,
        1, 2, w600_pwm_duty);

STATIC const mp_rom_map_elem_t w600_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&w600_pwm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&w600_pwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&w600_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty), MP_ROM_PTR(&w600_pwm_duty_obj) },
};

STATIC MP_DEFINE_CONST_DICT(w600_pwm_locals_dict,
                            w600_pwm_locals_dict_table);

const mp_obj_type_t machine_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = w600_pwm_print,
    .make_new = w600_pwm_make_new,
    .locals_dict = (mp_obj_dict_t *) &w600_pwm_locals_dict,
};

