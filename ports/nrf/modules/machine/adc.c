/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

ADC_HandleTypeDef ADCHandle0 = {.config.channel = 0};
ADC_HandleTypeDef ADCHandle1 = {.config.channel = 1};
ADC_HandleTypeDef ADCHandle2 = {.config.channel = 2};
ADC_HandleTypeDef ADCHandle3 = {.config.channel = 3};
ADC_HandleTypeDef ADCHandle4 = {.config.channel = 4};
ADC_HandleTypeDef ADCHandle5 = {.config.channel = 5};
ADC_HandleTypeDef ADCHandle6 = {.config.channel = 6};
ADC_HandleTypeDef ADCHandle7 = {.config.channel = 7};

STATIC const machine_adc_obj_t machine_adc_obj[] = {
    {{&machine_adc_type}, &ADCHandle0},
    {{&machine_adc_type}, &ADCHandle1},
    {{&machine_adc_type}, &ADCHandle2},
    {{&machine_adc_type}, &ADCHandle3},
    {{&machine_adc_type}, &ADCHandle4},
    {{&machine_adc_type}, &ADCHandle5},
    {{&machine_adc_type}, &ADCHandle6},
    {{&machine_adc_type}, &ADCHandle7},
};

STATIC int adc_find(mp_obj_t id) {
    // given an integer id
    int adc_id = mp_obj_get_int(id);

    int adc_idx = adc_id;

    if (adc_idx >= 0 && adc_idx <= MP_ARRAY_SIZE(machine_adc_obj)
        && machine_adc_obj[adc_idx].adc != NULL) {
        return adc_idx;
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
        "ADC(%d) does not exist", adc_id));
}


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
        { ARG_NEW_PIN,  MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1) } },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int adc_id = adc_find(args[ARG_NEW_PIN].u_obj);
    const machine_adc_obj_t *self = &machine_adc_obj[adc_id];

    return MP_OBJ_FROM_PTR(self);
}

/// \method value()
/// Read adc level.
mp_obj_t machine_adc_value(mp_obj_t self_in) {
    machine_adc_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(hal_adc_channel_value(&self->adc->config));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_machine_adc_value_obj, machine_adc_value);

/// \method battery_level()
/// Get battery level in percentage.
mp_obj_t machine_adc_battery_level(void) {
    return MP_OBJ_NEW_SMALL_INT(hal_adc_battery_level());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_machine_adc_battery_level_obj, machine_adc_battery_level);

STATIC const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&mp_machine_adc_value_obj) },

    // class methods
    { MP_ROM_QSTR(MP_QSTR_battery_level), MP_ROM_PTR(&mp_machine_adc_battery_level_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict, machine_adc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .make_new = machine_adc_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_adc_locals_dict,
    .print = machine_adc_print,
};

#endif // MICROPY_PY_MACHINE_ADC
