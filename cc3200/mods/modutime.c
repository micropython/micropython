/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "py/smallint.h"
#include "py/mphal.h"
#include "extmod/utime_mphal.h"
#include "timeutils.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"
#include "systick.h"
#include "pybrtc.h"
#include "mpexception.h"
#include "utils.h"

/// \module time - time related functions
///
/// The `time` module provides functions for getting the current time and date,
/// and for sleeping.

/******************************************************************************/
// Micro Python bindings

/// \function localtime([secs])
/// Convert a time expressed in seconds since Jan 1, 2000 into an 8-tuple which
/// contains: (year, month, mday, hour, minute, second, weekday, yearday)
/// If secs is not provided or None, then the current time from the RTC is used.
/// year includes the century (for example 2015)
/// month   is 1-12
/// mday    is 1-31
/// hour    is 0-23
/// minute  is 0-59
/// second  is 0-59
/// weekday is 0-6 for Mon-Sun.
/// yearday is 1-366
STATIC mp_obj_t time_localtime(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 0 || args[0] == mp_const_none) {
        timeutils_struct_time_t tm;

        // get the seconds from the RTC
        timeutils_seconds_since_2000_to_struct_time(pyb_rtc_get_seconds(), &tm);
        mp_obj_t tuple[8] = {
                mp_obj_new_int(tm.tm_year),
                mp_obj_new_int(tm.tm_mon),
                mp_obj_new_int(tm.tm_mday),
                mp_obj_new_int(tm.tm_hour),
                mp_obj_new_int(tm.tm_min),
                mp_obj_new_int(tm.tm_sec),
                mp_obj_new_int(tm.tm_wday),
                mp_obj_new_int(tm.tm_yday)
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        mp_int_t seconds = mp_obj_get_int(args[0]);
        timeutils_struct_time_t tm;
        timeutils_seconds_since_2000_to_struct_time(seconds, &tm);
        mp_obj_t tuple[8] = {
            tuple[0] = mp_obj_new_int(tm.tm_year),
            tuple[1] = mp_obj_new_int(tm.tm_mon),
            tuple[2] = mp_obj_new_int(tm.tm_mday),
            tuple[3] = mp_obj_new_int(tm.tm_hour),
            tuple[4] = mp_obj_new_int(tm.tm_min),
            tuple[5] = mp_obj_new_int(tm.tm_sec),
            tuple[6] = mp_obj_new_int(tm.tm_wday),
            tuple[7] = mp_obj_new_int(tm.tm_yday),
        };
        return mp_obj_new_tuple(8, tuple);
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(time_localtime_obj, 0, 1, time_localtime);

STATIC mp_obj_t time_mktime(mp_obj_t tuple) {
    mp_uint_t len;
    mp_obj_t *elem;

    mp_obj_get_array(tuple, &len, &elem);

    // localtime generates a tuple of len 8. CPython uses 9, so we accept both.
    if (len < 8 || len > 9) {
        mp_raise_TypeError(mpexception_num_type_invalid_arguments);
    }

    return mp_obj_new_int_from_uint(timeutils_mktime(mp_obj_get_int(elem[0]), mp_obj_get_int(elem[1]), mp_obj_get_int(elem[2]),
                                                     mp_obj_get_int(elem[3]), mp_obj_get_int(elem[4]), mp_obj_get_int(elem[5])));
}
MP_DEFINE_CONST_FUN_OBJ_1(time_mktime_obj, time_mktime);

STATIC mp_obj_t time_time(void) {
    return mp_obj_new_int(pyb_rtc_get_seconds());
}
MP_DEFINE_CONST_FUN_OBJ_0(time_time_obj, time_time);

STATIC mp_obj_t time_sleep(mp_obj_t seconds_o) {
    int32_t sleep_s = mp_obj_get_int(seconds_o);
    if (sleep_s > 0) {
        mp_hal_delay_ms(sleep_s * 1000);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

STATIC mp_obj_t time_sleep_ms (mp_obj_t ms_in) {
    mp_int_t ms = mp_obj_get_int(ms_in);
    if (ms > 0) {
        mp_hal_delay_ms(ms);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_ms_obj, time_sleep_ms);

STATIC mp_obj_t time_sleep_us (mp_obj_t usec_in) {
    mp_int_t usec = mp_obj_get_int(usec_in);
    if (usec > 0) {
        mp_hal_delay_us(usec);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_us_obj, time_sleep_us);

STATIC const mp_map_elem_t time_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_utime) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_localtime),       (mp_obj_t)&time_localtime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mktime),          (mp_obj_t)&time_mktime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_time),            (mp_obj_t)&time_time_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep),           (mp_obj_t)&time_sleep_obj },

    // MicroPython additions
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep_ms),        (mp_obj_t)&time_sleep_ms_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep_us),        (mp_obj_t)&time_sleep_us_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ticks_ms),        (mp_obj_t)&mp_utime_ticks_ms_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ticks_us),        (mp_obj_t)&mp_utime_ticks_us_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ticks_cpu),       (mp_obj_t)&mp_utime_ticks_cpu_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ticks_add),       (mp_obj_t)&mp_utime_ticks_add_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ticks_diff),      (mp_obj_t)&mp_utime_ticks_diff_obj },
};

STATIC MP_DEFINE_CONST_DICT(time_module_globals, time_module_globals_table);

const mp_obj_module_t mp_module_utime = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&time_module_globals,
};
