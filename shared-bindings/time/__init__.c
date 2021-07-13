/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#include "py/runtime.h"
#include "lib/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/shared/translate.h"

//| """time and timing related functions
//|
//| The `time` module is a strict subset of the CPython `cpython:time` module. So, code
//| written in MicroPython will work in CPython but not necessarily the other
//| way around."""
//|
//| def monotonic() -> float:
//|     """Returns an always increasing value of time with an unknown reference
//|     point. Only use it to compare against other values from `monotonic`.
//|
//|     :return: the current monotonic time
//|     :rtype: float"""
//|     ...
//|
STATIC mp_obj_t time_monotonic(void) {
    uint64_t ticks_ms = common_hal_time_monotonic_ms();
    return mp_obj_new_float(uint64_to_float(ticks_ms) / 1000.0f);
}
MP_DEFINE_CONST_FUN_OBJ_0(time_monotonic_obj, time_monotonic);

//| def sleep(seconds: float) -> None:
//|     """Sleep for a given number of seconds.
//|
//|     :param float seconds: the time to sleep in fractional seconds"""
//|     ...
//|
STATIC mp_obj_t time_sleep(mp_obj_t seconds_o) {
    #if MICROPY_PY_BUILTINS_FLOAT
    mp_float_t seconds = mp_obj_get_float(seconds_o);
    mp_float_t msecs = 1000.0f * seconds + 0.5f;
    #else
    mp_int_t seconds = mp_obj_get_int(seconds_o);
    mp_int_t msecs = 1000 * seconds;
    #endif
    if (seconds < 0) {
        mp_raise_ValueError(translate("sleep length must be non-negative"));
    }
    common_hal_time_delay_ms(msecs);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

#if MICROPY_PY_COLLECTIONS
mp_obj_t struct_time_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 1, 1, false);
    size_t len;
    mp_obj_t *items;
    mp_obj_get_array(args[0], &len, &items);
    if (len != 9) {
        mp_raise_TypeError(translate("time.struct_time() takes a 9-sequence"));
    }
    return namedtuple_make_new(type, len, items, NULL);
}

//| class struct_time:
//|     def __init__(self, time_tuple: Sequence[int]) -> None:
//|         """Structure used to capture a date and time.  Can be constructed from a `struct_time`, `tuple`, `list`, or `namedtuple` with 9 elements.
//|
//|         :param Sequence time_tuple: Sequence of time info: ``(tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_wday, tm_yday, tm_isdst)``
//|
//|           * ``tm_year``: the year, 2017 for example
//|           * ``tm_mon``: the month, range [1, 12]
//|           * ``tm_mday``: the day of the month, range [1, 31]
//|           * ``tm_hour``: the hour, range [0, 23]
//|           * ``tm_min``: the minute, range [0, 59]
//|           * ``tm_sec``: the second, range [0, 61]
//|           * ``tm_wday``: the day of the week, range [0, 6], Monday is 0
//|           * ``tm_yday``: the day of the year, range [1, 366], -1 indicates not known
//|           * ``tm_isdst``: 1 when in daylight savings, 0 when not, -1 if unknown."""
//|         ...
//|
const mp_obj_namedtuple_type_t struct_time_type_obj = {
    .base = {
        .base = {
            .type = &mp_type_type
        },
        .flags = MP_TYPE_FLAG_EXTENDED,
        .name = MP_QSTR_struct_time,
        .print = namedtuple_print,
        .parent = &mp_type_tuple,
        .make_new = struct_time_make_new,
        .attr = namedtuple_attr,
        MP_TYPE_EXTENDED_FIELDS(
            .unary_op = mp_obj_tuple_unary_op,
            .binary_op = mp_obj_tuple_binary_op,
            .subscr = mp_obj_tuple_subscr,
            .getiter = mp_obj_tuple_getiter,
            ),
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

mp_obj_t struct_time_from_tm(timeutils_struct_time_t *tm) {
    timeutils_struct_time_t tmp;
    mp_uint_t secs = timeutils_seconds_since_epoch(tm->tm_year, tm->tm_mon, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);
    timeutils_seconds_since_epoch_to_struct_time(secs, &tmp);
    tm->tm_wday = tmp.tm_wday;
    tm->tm_yday = tmp.tm_yday;

    mp_obj_t elems[9] = {
        mp_obj_new_int(tm->tm_year),
        mp_obj_new_int(tm->tm_mon),
        mp_obj_new_int(tm->tm_mday),
        mp_obj_new_int(tm->tm_hour),
        mp_obj_new_int(tm->tm_min),
        mp_obj_new_int(tm->tm_sec),
        mp_obj_new_int(tm->tm_wday),
        mp_obj_new_int(tm->tm_yday),
        mp_obj_new_int(-1), // tm_isdst is not supported
    };

    return namedtuple_make_new((const mp_obj_type_t *)&struct_time_type_obj, 9, elems, NULL);
};

void struct_time_to_tm(mp_obj_t t, timeutils_struct_time_t *tm) {
    mp_obj_t *elems;
    size_t len;

    if (!mp_obj_is_type(t, &mp_type_tuple) && !mp_obj_is_type(t, &struct_time_type_obj.base)) {
        mp_raise_TypeError(translate("Tuple or struct_time argument required"));
    }

    mp_obj_tuple_get(t, &len, &elems);
    if (len != 9) {
        mp_raise_TypeError(translate("function takes exactly 9 arguments"));
    }

    tm->tm_year = mp_obj_get_int(elems[0]);
    tm->tm_mon = mp_obj_get_int(elems[1]);
    tm->tm_mday = mp_obj_get_int(elems[2]);
    tm->tm_hour = mp_obj_get_int(elems[3]);
    tm->tm_min = mp_obj_get_int(elems[4]);
    tm->tm_sec = mp_obj_get_int(elems[5]);
    tm->tm_wday = mp_obj_get_int(elems[6]);
    tm->tm_yday = mp_obj_get_int(elems[7]);
    // elems[8] tm_isdst is not supported
}
#if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE
// Function to return a NotImplementedError on platforms that don't
// support long integers
STATIC mp_obj_t time_not_implemented(void) {
    mp_raise_NotImplementedError(translate("No long integer support"));
}
MP_DEFINE_CONST_FUN_OBJ_0(time_not_implemented_obj, time_not_implemented);
#endif

#if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
mp_obj_t MP_WEAK rtc_get_time_source_time(void) {
    mp_raise_RuntimeError(translate("RTC is not supported on this board"));
}

//| def time() -> int:
//|     """Return the current time in seconds since since Jan 1, 1970.
//|
//|     :return: the current time
//|     :rtype: int"""
//|     ...
//|
STATIC mp_obj_t time_time(void) {
    timeutils_struct_time_t tm;
    struct_time_to_tm(rtc_get_time_source_time(), &tm);
    mp_uint_t secs = timeutils_seconds_since_epoch(tm.tm_year, tm.tm_mon, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
    return mp_obj_new_int_from_uint(secs);
}
MP_DEFINE_CONST_FUN_OBJ_0(time_time_obj, time_time);

//| def monotonic_ns() -> int:
//|     """Return the time of the monotonic clock, cannot go backward, in nanoseconds.
//|
//|     :return: the current time
//|     :rtype: int"""
//|     ...
//|
STATIC mp_obj_t time_monotonic_ns(void) {
    uint64_t time64 = common_hal_time_monotonic_ns();
    return mp_obj_new_int_from_ll((long long)time64);
}
MP_DEFINE_CONST_FUN_OBJ_0(time_monotonic_ns_obj, time_monotonic_ns);

//| def localtime(secs: int) -> struct_time:
//|     """Convert a time expressed in seconds since Jan 1, 1970 to a struct_time in
//|     local time. If secs is not provided or None, the current time as returned
//|     by time() is used.
//|     The earliest date for which it can generate a time is Jan 1, 2000.
//|
//|     :return: the current time
//|     :rtype: time.struct_time"""
//|     ...
//|
STATIC mp_obj_t time_localtime(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0 || args[0] == mp_const_none) {
        return rtc_get_time_source_time();
    }

    mp_obj_t arg = args[0];
    if (mp_obj_is_float(arg)) {
        arg = mp_obj_new_int_from_float(mp_obj_get_float(arg));
    }

    mp_int_t secs = mp_obj_get_int(arg);

    if (secs < 0 || (mp_uint_t)secs < TIMEUTILS_SECONDS_1970_TO_2000) {
        mp_raise_msg(&mp_type_OverflowError, translate("timestamp out of range for platform time_t"));
    }

    timeutils_struct_time_t tm;
    timeutils_seconds_since_epoch_to_struct_time(secs, &tm);

    return struct_time_from_tm(&tm);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(time_localtime_obj, 0, 1, time_localtime);

//| def mktime(t: struct_time) -> int:
//|     """This is the inverse function of localtime(). Its argument is the
//|     struct_time or full 9-tuple (since the dst flag is needed; use -1 as the
//|     dst flag if it is unknown) which expresses the time in local time, not UTC.
//|     The earliest date for which it can generate a time is Jan 1, 2000.
//|
//|     :return: seconds
//|     :rtype: int"""
//|     ...
//|
STATIC mp_obj_t time_mktime(mp_obj_t t) {
    mp_obj_t *elem;
    size_t len;

    if (!mp_obj_is_type(t, &mp_type_tuple) && !mp_obj_is_type(t, &struct_time_type_obj.base)) {
        mp_raise_TypeError(translate("Tuple or struct_time argument required"));
    }

    mp_obj_tuple_get(t, &len, &elem);
    if (len != 9) {
        mp_raise_TypeError_varg(translate("function takes %d positional arguments but %d were given"), 9, len);
    }

    if (mp_obj_get_int(elem[0]) < 2000) {
        mp_raise_msg(&mp_type_OverflowError, translate("timestamp out of range for platform time_t"));
    }

    mp_uint_t secs = timeutils_mktime(mp_obj_get_int(elem[0]), mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]),
        mp_obj_get_int(elem[3]), mp_obj_get_int(elem[4]), mp_obj_get_int(elem[5]));
    return mp_obj_new_int_from_uint(secs);
}
MP_DEFINE_CONST_FUN_OBJ_1(time_mktime_obj, time_mktime);
#endif // MICROPY_LONGINT_IMPL
#endif // MICROPY_PY_COLLECTIONS

STATIC const mp_rom_map_elem_t time_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_time) },

    { MP_ROM_QSTR(MP_QSTR_monotonic), MP_ROM_PTR(&time_monotonic_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&time_sleep_obj) },
    #if MICROPY_PY_COLLECTIONS
    { MP_ROM_QSTR(MP_QSTR_struct_time), MP_ROM_PTR(&struct_time_type_obj) },
    #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
    { MP_ROM_QSTR(MP_QSTR_localtime), MP_ROM_PTR(&time_localtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_mktime), MP_ROM_PTR(&time_mktime_obj) },
    #endif // MICROPY_LONGINT_IMPL
    #endif // MICROPY_PY_COLLECTIONS
    #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&time_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_monotonic_ns), MP_ROM_PTR(&time_monotonic_ns_obj) },
    #endif
    #if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE
    { MP_ROM_QSTR(MP_QSTR_localtime), MP_ROM_PTR(&time_not_implemented_obj) },
    { MP_ROM_QSTR(MP_QSTR_mktime), MP_ROM_PTR(&time_not_implemented_obj) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&time_not_implemented_obj) },
    { MP_ROM_QSTR(MP_QSTR_monotonic_ns), MP_ROM_PTR(&time_not_implemented_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(time_module_globals, time_module_globals_table);

const mp_obj_module_t time_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&time_module_globals,
};
