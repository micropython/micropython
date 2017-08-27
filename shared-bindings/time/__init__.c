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

#include "py/obj.h"
#include "py/objnamedtuple.h"
#include "shared-bindings/time/__init__.h"

//| :mod:`time` --- time and timing related functions
//| ========================================================
//|
//| .. module:: time
//|   :synopsis: time and timing related functions
//|   :platform: SAMD21
//|
//| The `time` module is a strict subset of the CPython `cpython:time` module. So, code
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
    return mp_obj_new_float(common_hal_time_monotonic() / 1000.0);
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
    float seconds = mp_obj_get_float(seconds_o);
    #else
    int seconds = mp_obj_get_int(seconds_o);
    #endif
    if (seconds < 0) {
        mp_raise_ValueError("sleep length must be non-negative");
    }
    common_hal_time_delay_ms(1000 * seconds);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

#if MICROPY_PY_COLLECTIONS
mp_obj_t struct_time_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    if (n_args != 1) {
        mp_raise_TypeError("time.struct_time() takes exactly 1 argument");
    }
    if (!MP_OBJ_IS_TYPE(args[0], &mp_type_tuple) || ((mp_obj_tuple_t*) MP_OBJ_TO_PTR(args[0]))->len != 9) {
        mp_raise_TypeError("time.struct_time() takes a 9-sequence");
    }

    mp_obj_tuple_t* tuple = MP_OBJ_TO_PTR(args[0]);
    return namedtuple_make_new(type, 9, 0, tuple->items);
}

//| .. class:: struct_time((tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_wday, tm_yday, tm_isdst))
//|
//|   Structure used to capture a date and time. Note that it takes a tuple!
//|
//|   :param int tm_year: the year, 2017 for example
//|   :param int tm_mon: the month, range [1, 12]
//|   :param int tm_mday: the day of the month, range [1, 31]
//|   :param int tm_hour: the hour, range [0, 23]
//|   :param int tm_min: the minute, range [0, 59]
//|   :param int tm_sec: the second, range [0, 61]
//|   :param int tm_wday: the day of the week, range [0, 6], Monday is 0
//|   :param int tm_yday: the day of the year, range [1, 366], -1 indicates not known
//|   :param int tm_isdst: 1 when in daylight savings, 0 when not, -1 if unknown.
//|
const mp_obj_namedtuple_type_t struct_time_type_obj = {
    .base = {
        .base = {
            .type = &mp_type_type
        },
        .name = MP_QSTR_struct_time,
        .print = namedtuple_print,
        .make_new = struct_time_make_new,
        .unary_op = mp_obj_tuple_unary_op,
        .binary_op = mp_obj_tuple_binary_op,
        .attr = namedtuple_attr,
        .subscr = mp_obj_tuple_subscr,
        .getiter = mp_obj_tuple_getiter,
        .parent = &mp_type_tuple,
    },
    .n_fields = 9,
    .fields = {
        MP_QSTR_tm_year,
        MP_QSTR_tm_mon,
        MP_QSTR_tm_mday,
        MP_QSTR_tm_hour,
        MP_QSTR_tm_min,
        MP_QSTR_tm_sec,
        MP_QSTR_tm_wday,
        MP_QSTR_tm_yday,
        MP_QSTR_tm_isdst
    },
};
#endif

STATIC const mp_rom_map_elem_t time_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_time) },

    { MP_ROM_QSTR(MP_QSTR_monotonic), MP_ROM_PTR(&time_monotonic_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&time_sleep_obj) },
    #if MICROPY_PY_COLLECTIONS
    { MP_ROM_QSTR(MP_QSTR_struct_time), MP_ROM_PTR(&struct_time_type_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(time_module_globals, time_module_globals_table);

const mp_obj_module_t time_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&time_module_globals,
};
