/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#include "py/runtime.h"
#include "extmod/utime_mphal.h"

// time module mostly compatible with CPython's time. For reference, this is the CPython
// struct_time from https://docs.python.org/3/library/time.html#time.struct_time:
// Index Attribute Values
//   0   tm_year   (for example, 1993)
//   1   tm_mon    range [1, 12]
//   2   tm_mday   range [1, 31]
//   3   tm_hour   range [0, 23]
//   4   tm_min    range [0, 59]
//   5   tm_sec    range [0, 61]; see (2) in strftime() description
//   6   tm_wday   range [0, 6], Monday is 0
//   7   tm_yday   range [1, 366]
//   8   tm_isdst  0=no, 1=yes, -1=unknown
// N/A   tm_zone   abbreviation of timezone name
// N/A   tm_gmtoff offset east of UTC in seconds
// This module supports the first 9 elements (i.e. a 9-tuple) without names.

// convert a python 9-tuple to a struct tm
STATIC void time_tm_from_tuple(const mp_obj_t tuple, struct tm *tm) {
    size_t len;
    mp_obj_t *elem;
    mp_obj_get_array(tuple, &len, &elem);

    if (len != 9) {
        mp_raise_msg_varg(&mp_type_TypeError, MP_ERROR_TEXT("9-tuple required (%d given)"), len);
    }

    time_t wday = mp_obj_get_int(elem[6]) + 1;
    if (wday > 7) {
        wday = 0;
    }
    tm->tm_year = mp_obj_get_int(elem[0]) - 1900;
    tm->tm_mon = mp_obj_get_int(elem[1]) - 1;
    tm->tm_mday = mp_obj_get_int(elem[2]);
    tm->tm_hour = mp_obj_get_int(elem[3]);
    tm->tm_min = mp_obj_get_int(elem[4]);
    tm->tm_sec = mp_obj_get_int(elem[5]);
    tm->tm_wday = wday;
    tm->tm_yday = mp_obj_get_int(elem[7]) - 1;
    tm->tm_isdst = mp_obj_get_int(elem[8]);
}

// convert a struct tm to a python 9-tuple
STATIC mp_obj_t *time_tm_to_tuple(const struct tm *tm) {
    mp_obj_t tuple[9] = {
        tuple[0] = mp_obj_new_int(1900 + tm->tm_year),
        tuple[1] = mp_obj_new_int(tm->tm_mon + 1),
        tuple[2] = mp_obj_new_int(tm->tm_mday),
        tuple[3] = mp_obj_new_int(tm->tm_hour),
        tuple[4] = mp_obj_new_int(tm->tm_min),
        tuple[5] = mp_obj_new_int(tm->tm_sec),
        tuple[6] = mp_obj_new_int(tm->tm_wday == 0 ? 7 : tm->tm_wday - 1),
        tuple[7] = mp_obj_new_int(tm->tm_yday + 1),
        tuple[8] = mp_obj_new_int(tm->tm_isdst),
    };
    return mp_obj_new_tuple(8, tuple);
}

STATIC time_t time_get_seconds(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0 || args[0] == mp_const_none) {
        struct timeval tv;
        if (gettimeofday(&tv, NULL) != 0) {
            mp_raise_OSError(errno);
        }
        return tv.tv_sec;
    } else {
        return mp_obj_get_int(args[0]);
    }
}

STATIC mp_obj_t time_gmtime(size_t n_args, const mp_obj_t *args) {
    time_t seconds = time_get_seconds(n_args, args);
    struct tm tm;
    gmtime_r(&seconds, &tm);
    return time_tm_to_tuple(&tm);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(time_gmtime_obj, 0, 1, time_gmtime);

STATIC mp_obj_t time_localtime(size_t n_args, const mp_obj_t *args) {
    time_t seconds = time_get_seconds(n_args, args);
    struct tm tm;
    localtime_r(&seconds, &tm);
    return time_tm_to_tuple(&tm);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(time_localtime_obj, 0, 1, time_localtime);

STATIC mp_obj_t time_mktime(mp_obj_t tuple) {
    struct tm tm;
    time_tm_from_tuple(tuple, &tm);

    time_t seconds = mktime(&tm);
    if (seconds == -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid input"));
    }
    return MP_OBJ_NEW_SMALL_INT(seconds);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(time_mktime_obj, time_mktime);

STATIC mp_obj_t time_time(void) {
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        mp_raise_OSError(errno);
    }
    return mp_obj_new_int(tv.tv_sec);
}
MP_DEFINE_CONST_FUN_OBJ_0(time_time_obj, time_time);

STATIC mp_obj_t time_tzset(mp_obj_t tz) {
    // tz is something like PST+8PDT,M3.2.0/2,M11.1.0/2
    const char *zone = mp_obj_str_get_str(tz);
    setenv("TZ", zone, 1);
    tzset();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_tzset_obj, time_tzset);

STATIC mp_obj_t time_set_time(const mp_obj_t seconds_in) {
    struct timeval tv = { mp_obj_get_int(seconds_in), 0 };
    if (settimeofday(&tv, NULL) != 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_set_time_obj, time_set_time);

STATIC mp_obj_t time_adjtime(const mp_obj_t microseconds_in) {
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = mp_obj_get_int(microseconds_in),
    };
    // turn microseconds into seconds+microseconds
    if (tv.tv_usec >= 1000000) {
        tv.tv_sec = tv.tv_usec / 1000000;
        tv.tv_usec = tv.tv_usec - tv.tv_sec * 1000000;
    } else if (tv.tv_usec <= -1000000) {
        tv.tv_sec = -(-tv.tv_usec / 1000000); // round to zero
        tv.tv_usec = tv.tv_usec - tv.tv_sec * 1000000; // negative remainder
    }

    struct timeval tv_old;
    if (adjtime(&tv, &tv_old) != 0) {
        mp_raise_OSError(errno);
    }
    return mp_obj_new_int(tv.tv_sec * 1000000 + tv.tv_usec);
}
MP_DEFINE_CONST_FUN_OBJ_1(time_adjtime_obj, time_adjtime);

STATIC const mp_rom_map_elem_t time_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_utime) },

    { MP_ROM_QSTR(MP_QSTR_gmtime), MP_ROM_PTR(&time_gmtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_localtime), MP_ROM_PTR(&time_localtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_mktime), MP_ROM_PTR(&time_mktime_obj) },
    { MP_ROM_QSTR(MP_QSTR_tzset), MP_ROM_PTR(&time_tzset_obj) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&time_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_time), MP_ROM_PTR(&time_set_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_adjtime), MP_ROM_PTR(&time_adjtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&mp_utime_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_ms), MP_ROM_PTR(&mp_utime_sleep_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep_us), MP_ROM_PTR(&mp_utime_sleep_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_ms), MP_ROM_PTR(&mp_utime_ticks_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_us), MP_ROM_PTR(&mp_utime_ticks_us_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_cpu), MP_ROM_PTR(&mp_utime_ticks_cpu_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_add), MP_ROM_PTR(&mp_utime_ticks_add_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_diff), MP_ROM_PTR(&mp_utime_ticks_diff_obj) },
};

STATIC MP_DEFINE_CONST_DICT(time_module_globals, time_module_globals_table);

const mp_obj_module_t utime_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&time_module_globals,
};
