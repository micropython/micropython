/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ernesto Gigliotti <ernestogigliotti@gmail.com>
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

#include "py/runtime.h"
#include "board.h"
#include "modpyb.h"
#include "ciaanxp_mphal.h"

typedef struct _pyb_adc_obj_t {
    mp_obj_base_t base;
} pyb_adc_obj_t;

STATIC pyb_adc_obj_t pyb_adc_obj[] = {
    {{&pyb_adc_type}},
    {{&pyb_adc_type}},
    {{&pyb_adc_type}},
};

#define NUM_ADC_CHN MP_ARRAY_SIZE(pyb_adc_obj)
#define ADC_ID(obj) ((obj) - &pyb_adc_obj[0] + 1)

void pyb_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_adc_obj_t *self = self_in;
    mp_printf(print, "ADC channel(%u)", ADC_ID(self));
}

STATIC mp_obj_t pyb_adc_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t adc_id = mp_obj_get_int(args[0]);

    if (adc_id<=0 || adc_id>NUM_ADC_CHN) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "ADC channel %d does not exist", adc_id));
    }

    return (mp_obj_t)&pyb_adc_obj[adc_id-1];
}

// read method : read an ADC channel value after a conversion
mp_obj_t pyb_adc_read(mp_obj_t self_in) {
    pyb_adc_obj_t *self = self_in;

    mp_hal_enableADCchannel(ADC_ID(self));
    mp_hal_startADCconversion();
    uint16_t val =  mp_hal_readADCchannel(ADC_ID(self));
    return MP_OBJ_NEW_SMALL_INT(val);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_adc_read_obj, pyb_adc_read);


STATIC const mp_map_elem_t pyb_adc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&pyb_adc_read_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_adc_locals_dict, pyb_adc_locals_dict_table);

const mp_obj_type_t pyb_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .print = pyb_adc_print,
    .make_new = pyb_adc_make_new,
    .locals_dict = (mp_obj_t)&pyb_adc_locals_dict,
};
