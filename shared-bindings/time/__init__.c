/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Josef Gajdusek
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include <string.h>

//#include "py/nlr.h"
#include "py/obj.h"
//#include "py/gc.h"
//#include "py/runtime.h"
//#include "py/mphal.h"
//#include "py/smallint.h"
#include "shared-bindings/time/__init__.h"

//| :mod:`time` --- time and timing related functions
//| ========================================================
//|
//| .. module:: time
//|   :synopsis: time and timing related functions
//|   :platform: SAMD21
//|
//| The `time` module is a strict subset of the CPython `time` module. So, code
//| written in MicroPython will work in CPython but not necessarily the other
//| way around.
//|
//| .. method:: monotonic()
//|
//|   Returns an always increasing value of time with an unknown reference
//|   point. Only use it to compare against other values from `monotonic`.
//|
//|   :return: the current monotonic time
//|   :rtype: float
//|
STATIC mp_obj_t time_monotonic(void) {
    return mp_obj_new_float(common_hal_time_monotonic() / 100.0);
}
MP_DEFINE_CONST_FUN_OBJ_0(time_monotonic_obj, time_monotonic);

//| .. method:: sleep(seconds)
//|
//|   Sleep for a given number of seconds.
//|
//|   :param float seconds: the time to sleep in fractional seconds
//|
STATIC mp_obj_t time_sleep(mp_obj_t seconds_o) {
    #if MICROPY_PY_BUILTINS_FLOAT
    common_hal_time_delay_ms(1000 * mp_obj_get_float(seconds_o));
    #else
    common_hal_time_delay_ms(1000 * mp_obj_get_int(seconds_o));
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

STATIC const mp_map_elem_t time_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_time) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_monotonic), (mp_obj_t)&time_monotonic_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep), (mp_obj_t)&time_sleep_obj },
};

STATIC MP_DEFINE_CONST_DICT(time_module_globals, time_module_globals_table);

const mp_obj_module_t time_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&time_module_globals,
};
