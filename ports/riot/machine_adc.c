/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Koen Zandberg
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
#include "periph_conf.h"
#include "periph/adc.h"

#include "py/runtime.h"
#include "modmachine.h"

#if MICROPY_PY_MACHINE_ADC

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    adc_t channel;
} machine_adc_obj_t;

typedef struct _adc_res_helper {
    adc_res_t res_symbol;
    int resolution;
} adc_res_helper_t;

STATIC machine_adc_obj_t machine_adc_obj[ADC_NUMOF];

STATIC const adc_res_helper_t adc_res_helper[] = {
    { ADC_RES_16BIT, 16 },
    { ADC_RES_14BIT, 14 },
    { ADC_RES_12BIT, 12 },
    { ADC_RES_10BIT, 10 },
    { ADC_RES_8BIT, 8 },
    // 6 bit support intentionally omitted to keep the taylor expansion below
    // simple
};

STATIC void machine_adc_print(const mp_print_t *print,
                              mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = self_in;
    mp_printf(print, "ADC(%u)", self->channel);
}

STATIC mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    machine_adc_obj_t *self = self_in;

    // Iterate over the possible resolutions until a valid resolution is used.
    for (size_t i = 0; i < MP_ARRAY_SIZE(adc_res_helper); i++) {
        int val = adc_sample(self->channel, adc_res_helper[i].res_symbol);
        if (val >= 0) {
            int resolution = adc_res_helper[i].resolution;
            // Taylor expansion to scale the result to the full 16 bit range
            uint16_t scaled = val << (16 - resolution) |
                              val >> (2 * resolution - 16);
            return MP_OBJ_NEW_SMALL_INT(scaled);
        }
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
        "ADC(%u) sampling error", self->channel));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

STATIC mp_obj_t machine_adc_make_new(const mp_obj_type_t *type, size_t n_args,
                                     size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int line = args[ARG_id].u_int;

    if ((line < 0) || ((size_t)line >= ADC_NUMOF)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "ADC(%d) is an invalid line for port", line));
    }

    machine_adc_obj_t *self = &machine_adc_obj[(size_t)line];
    self->base.type = &machine_adc_type;
    self->channel = (adc_t)line;

    if (adc_init(line) < 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "Unable to initialize ADC(%d)", line));
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&machine_adc_read_u16_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict,
                            machine_adc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = machine_adc_print,
    .make_new = machine_adc_make_new,
    .locals_dict = (mp_obj_t)&machine_adc_locals_dict,
};

#endif /* MICROPY_PY_MACHINE_ADC */
