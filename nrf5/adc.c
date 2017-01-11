/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "adc.h"
#include "hal_adc.h"

#if MICROPY_PY_MACHINE_ADC

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    ADC_HandleTypeDef *adc;
} machine_adc_obj_t;

/// \method __str__()
/// Return a string describing the ADC object.
STATIC void machine_adc_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_adc_obj_t *self = o;

    (void)self;

    mp_printf(print, "ADC()");
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

// for make_new
enum {
    ARG_NEW_PIN,
};

STATIC mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { ARG_NEW_PIN,  MP_ARG_REQUIRED | MP_ARG_INT },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_adc_obj_t *s = m_new_obj(machine_adc_obj_t);
    s->base.type = type;

    mp_int_t channel_num;

    if (args[ARG_NEW_PIN].u_int > 0) {
        channel_num = args[ARG_NEW_PIN].u_int;
    } else {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "Channel number not set"));
    }

    s->adc->init.channel = channel_num;

    hal_adc_init(ADC_BASE, &s->adc->init);

    return MP_OBJ_FROM_PTR(s);
}

/// \method init()
mp_obj_t machine_adc_init(mp_obj_t self_in) {
    hal_adc_start(ADC_BASE);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_adc_init_obj, machine_adc_init);

/// \method deinit()
mp_obj_t machine_adc_deinit(mp_obj_t self_in) {
    hal_adc_stop(ADC_BASE);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_adc_deinit_obj, machine_adc_deinit);

/// \method value()
/// Read adc level.
mp_obj_t machine_adc_value(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(hal_adc_value(ADC_BASE));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_adc_value_obj, machine_adc_value);

/// \method battery_level()
/// Get battery level in percentage.
mp_obj_t machine_adc_battery_level(void) {
    return MP_OBJ_NEW_SMALL_INT(hal_adc_battery_level(ADC_BASE));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_machine_adc_battery_level_obj, machine_adc_battery_level);

STATIC const mp_map_elem_t machine_adc_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&mp_machine_adc_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&mp_machine_adc_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_value), (mp_obj_t)&mp_machine_adc_value_obj },

    // class methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_battery_level), (mp_obj_t)&mp_machine_adc_battery_level_obj },
};

STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .make_new = machine_adc_make_new,
    .locals_dict = (mp_obj_t)&machine_adc_locals_dict,
    .print = machine_adc_print,
};

#endif // MICROPY_PY_MACHINE_ADC
