/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include "py/gc.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/smallint.h"

/// \module time - time related functions
///
/// The `time` module provides functions for getting the current time and date,
/// and for sleeping.

/// \function sleep(seconds)
/// Sleep for the given number of seconds.
STATIC mp_obj_t time_sleep(mp_obj_t seconds_o) {
    #if MICROPY_PY_BUILTINS_FLOAT
    mp_hal_delay_ms(1000 * mp_obj_get_float(seconds_o));
    #else
    mp_hal_delay_ms(1000 * mp_obj_get_int(seconds_o));
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

STATIC mp_obj_t time_sleep_ms(mp_obj_t arg) {
    mp_hal_delay_ms(mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_ms_obj, time_sleep_ms);

STATIC mp_obj_t time_sleep_us(mp_obj_t arg) {
    mp_hal_delay_us(mp_obj_get_int(arg));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_us_obj, time_sleep_us);

STATIC const mp_map_elem_t time_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_utime) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep), (mp_obj_t)&time_sleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep_ms), (mp_obj_t)&time_sleep_ms_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep_us), (mp_obj_t)&time_sleep_us_obj },
};

STATIC MP_DEFINE_CONST_DICT(time_module_globals, time_module_globals_table);

const mp_obj_module_t utime_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&time_module_globals,
};
