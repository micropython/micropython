/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Vladimir Dronnikov
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

#include "py/nlr.h"
#include "py/obj.h"
#include "py/gc.h"
#include "gccollect.h"
#include "pyexec.h"
#include "pybstdio.h"
#include MICROPY_HAL_H
#include "user_interface.h"

#include <gpio.h>

STATIC mp_obj_t gpio_output(mp_obj_t pin, mp_obj_t value) {
    GPIO_OUTPUT_SET(mp_obj_get_int(pin), mp_obj_get_int(value));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(gpio_output_obj, gpio_output);

/* TODO: pullup/pulldown
PIN_PULLDWN_DIS(PERIPHS_IO_MUX_GPIO0_U);
PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);
*/
/* TODO: opendrain/pushpull, eg https://github.com/mattcallow/esp8266-sdk/blob/master/apps/07switch/driver/i2c_master.c#L111
*/
STATIC mp_obj_t gpio_input(mp_obj_t pin) {
    GPIO_DIS_OUTPUT(mp_obj_get_int(pin));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_input_obj, gpio_input);

STATIC mp_obj_t gpio_get(mp_obj_t pin) {
    return MP_OBJ_NEW_SMALL_INT(GPIO_INPUT_GET(mp_obj_get_int(pin)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(gpio_get_obj, gpio_get);

STATIC const mp_map_elem_t gpio_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_gpio) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_output), (mp_obj_t)&gpio_output_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&gpio_input_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_get), (mp_obj_t)&gpio_get_obj },
};

STATIC MP_DEFINE_CONST_DICT(gpio_module_globals, gpio_module_globals_table);

const mp_obj_module_t gpio_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_gpio,
    .globals = (mp_obj_dict_t*)&gpio_module_globals,
};
