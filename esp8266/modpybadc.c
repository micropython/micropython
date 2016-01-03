/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Josef Gajdusek
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
#include "py/obj.h"
#include "py/runtime.h"
#include "user_interface.h"

const mp_obj_type_t pyb_adc_type;

typedef struct _pyb_adc_obj_t {
    mp_obj_base_t base;
    bool isvdd;
} pyb_adc_obj_t;

STATIC pyb_adc_obj_t pyb_adc_vdd3 = {{&pyb_adc_type}, true};
STATIC pyb_adc_obj_t pyb_adc_adc = {{&pyb_adc_type}, false};

STATIC mp_obj_t pyb_adc_make_new(const mp_obj_type_t *type_in, mp_uint_t n_args, mp_uint_t n_kw,
    const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_int_t chn = mp_obj_get_int(args[0]);

    switch (chn) {
    case 0:
        return &pyb_adc_adc;
    case 1:
        return &pyb_adc_vdd3;
    default:
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "not a valid ADC Channel: %d", chn));
    }
}

STATIC mp_obj_t pyb_adc_read(mp_obj_t self_in) {
    pyb_adc_obj_t *adc = self_in;

    if (adc->isvdd) {
        return mp_obj_new_int(system_get_vdd33());
    } else {
        return mp_obj_new_int(system_adc_read());
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_adc_read_obj, pyb_adc_read);

STATIC const mp_map_elem_t pyb_adc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&pyb_adc_read_obj }
};
STATIC MP_DEFINE_CONST_DICT(pyb_adc_locals_dict, pyb_adc_locals_dict_table);

const mp_obj_type_t pyb_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .make_new = pyb_adc_make_new,
    .locals_dict = (mp_obj_t)&pyb_adc_locals_dict,
};
