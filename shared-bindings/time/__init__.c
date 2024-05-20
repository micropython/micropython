// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2015 Josef Gajdusek
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/obj.h"
#include "py/objnamedtuple.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/time/__init__.h"

//| """time and timing related functions
//|
//| |see_cpython_module| :mod:`cpython:time`.
//| """
//|
//| def monotonic() -> float:
//|     """Returns an always increasing value of time with an unknown reference
//|     point. Only use it to compare against other values from `time.monotonic()`
//|     during the same code run.
//|
//|     On most boards, `time.monotonic()` converts a 64-bit millisecond tick counter
//|     to a float. Floats on most boards are encoded in 30 bits internally, with
//|     effectively 22 bits of precision. The float returned by `time.monotonic()` will
//|     accurately represent time to millisecond precision only up to 2**22 milliseconds
//|     (about 1.165 hours).
//|     At that point it will start losing precision, and its value will change only
//|     every second millisecond. At 2**23 milliseconds it will change every fourth
//|     millisecond, and so forth.
//|
//|     If you need more consistent precision, use `time.monotonic_ns()`, or `supervisor.ticks_ms()`.
//|     `time.monotonic_ns()` is not available on boards without long integer support.
//|     `supervisor.ticks_ms()` uses intervals of a millisecond, but wraps around, and is not
//|     CPython-compatible.
//|
//|     :return: the current monotonic time
//|     :rtype: float"""
//|     ...
//|
static mp_obj_t time_monotonic(void) {
    uint64_t ticks_ms = common_hal_time_monotonic_ms();
    return mp_obj_new_float(uint64_to_float(ticks_ms) / MICROPY_FLOAT_CONST(1000.0));
}
MP_DEFINE_CONST_FUN_OBJ_0(time_monotonic_obj, time_monotonic);

//| def sleep(seconds: float) -> None:
//|     """Sleep for a given number of seconds.
//|
//|     :param float seconds: the time to sleep in fractional seconds"""
//|     ...
//|
static mp_obj_t time_sleep(mp_obj_t seconds_o) {
    #if MICROPY_PY_BUILTINS_FLOAT
    mp_float_t seconds = mp_obj_get_float(seconds_o);
    mp_float_t msecs = MICROPY_FLOAT_CONST(1000.0) * seconds + MICROPY_FLOAT_CONST(0.5);
    #else
    mp_int_t seconds = mp_obj_get_int(seconds_o);
    mp_int_t msecs = 1000 * seconds;
    #endif
    mp_arg_validate_int_min(msecs, 0, MP_QSTR_seconds);
    common_hal_time_delay_ms(msecs);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

#if MICROPY_PY_COLLECTIONS
static mp_obj_t struct_time_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    size_t len;
    mp_obj_t *items;
    mp_obj_get_array(args[0], &len, &items);
    mp_arg_validate_length(len, 9, MP_QSTR_value);
    return namedtuple_make_new(type, len, 0, items);
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
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS_MAKE_NEW(MP_QSTR_struct_time, struct_time_make_new),
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

    return namedtuple_make_new((const mp_obj_type_t *)&struct_time_type_obj, 9, 0, elems);
};

void struct_time_to_tm(mp_obj_t t, timeutils_struct_time_t *tm) {
    mp_obj_t *elems;
    size_t len;

    if (!mp_obj_is_type(t, &mp_type_tuple)) {
        mp_arg_validate_type(t, (mp_obj_type_t *)&struct_time_type_obj.base, MP_QSTR_value);
    }

    mp_obj_tuple_get(t, &len, &elems);
    if (len != 9) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("function takes %d positional arguments but %d were given"), 9, len);
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
static mp_obj_t time_not_implemented(void) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("No long integer support"));
}
MP_DEFINE_CONST_FUN_OBJ_0(time_not_implemented_obj, time_not_implemented);
#endif

#if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
mp_obj_t MP_WEAK rtc_get_time_source_time(void) {
    mp_raise_RuntimeError(MP_ERROR_TEXT("RTC is not supported on this board"));
}

//| def time() -> int:
//|     """Return the current time in seconds since since Jan 1, 1970.
//|
//|     :return: the current time
//|     :rtype: int"""
//|     ...
//|
static mp_obj_t time_time(void) {
    timeutils_struct_time_t tm;
    struct_time_to_tm(rtc_get_time_source_time(), &tm);
    mp_uint_t secs = timeutils_seconds_since_epoch(tm.tm_year, tm.tm_mon, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
    return mp_obj_new_int_from_uint(secs);
}
MP_DEFINE_CONST_FUN_OBJ_0(time_time_obj, time_time);

//| def monotonic_ns() -> int:
//|     """Return the time of the monotonic clock, which cannot go backward, in nanoseconds.
//|     Not available on boards without long integer support.
//|     Only use it to compare against other values from `time.monotonic()`
//|     during a single code run.
//|
//|     :return: the current time
//|     :rtype: int"""
//|     ...
//|
static mp_obj_t time_monotonic_ns(void) {
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
static mp_obj_t time_localtime(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0 || args[0] == mp_const_none) {
        return rtc_get_time_source_time();
    }

    mp_obj_t arg = args[0];
    if (mp_obj_is_float(arg)) {
        arg = mp_obj_new_int_from_float(mp_obj_get_float(arg));
    }

    mp_int_t secs = mp_obj_get_int(arg);

    #if MICROPY_EPOCH_IS_1970
    if (secs < 0 || (mp_uint_t)secs < TIMEUTILS_SECONDS_1970_TO_2000) {
    #else
    if (secs < 0) {
        #endif
        mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("timestamp out of range for platform time_t"));
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
static mp_obj_t time_mktime(mp_obj_t t) {
    mp_obj_t *elem;
    size_t len;

    if (!mp_obj_is_type(t, &mp_type_tuple)) {
        mp_arg_validate_type(t, (mp_obj_type_t *)&struct_time_type_obj.base, MP_QSTR_value);
    }

    mp_obj_tuple_get(t, &len, &elem);
    if (len != 9) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("function takes %d positional arguments but %d were given"), 9, len);
    }

    if (mp_obj_get_int(elem[0]) < 2000) {
        mp_raise_msg_varg(&mp_type_OverflowError, MP_ERROR_TEXT("%q out of range"), MP_QSTR_tm_year);
    }

    mp_uint_t secs = timeutils_mktime(mp_obj_get_int(elem[0]), mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]),
        mp_obj_get_int(elem[3]), mp_obj_get_int(elem[4]), mp_obj_get_int(elem[5]));
    return mp_obj_new_int_from_uint(secs);
}
MP_DEFINE_CONST_FUN_OBJ_1(time_mktime_obj, time_mktime);
#endif // MICROPY_LONGINT_IMPL
#endif // MICROPY_PY_COLLECTIONS

static const mp_rom_map_elem_t time_module_globals_table[] = {
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

static MP_DEFINE_CONST_DICT(time_module_globals, time_module_globals_table);

const mp_obj_module_t time_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&time_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_time, time_module);
