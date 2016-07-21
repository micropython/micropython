/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include "py/binary.h"
#include "py/mphal.h"
#include "adc.h"
#include "pin.h"
#include "genhdr/pins.h"

STATIC const uint16_t analog_pin_table[] = {A0, A1, A2, A3, A4, A5, A6};

/// \method read()
/// Read the value on the analog pin and return it.  The returned value
/// will be between 0 and 4095.
STATIC mp_obj_t adc_read(mp_obj_t self_in) {
	int i = 0;
    pin_obj_t *self = self_in;
    uint16_t pin = pin_mapping(self);
    for(i = 0; i < 7; i++){
    	if(analog_pin_table[i] == pin){
    	    uint32_t data = analogRead(pin);
    	    return mp_obj_new_int(data);
    	}
    }
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(adc_read_obj, adc_read);

STATIC const mp_map_elem_t adc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&adc_read_obj},
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t pyb_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .locals_dict = (mp_obj_t)&adc_locals_dict,
};
