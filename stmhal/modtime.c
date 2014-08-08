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

#include "stm32f4xx_hal.h"

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "portmodules.h"
#include "rtc.h"

/// \module time - time related functions
///
/// The `time` module provides functions for getting the current time and date,
/// and for sleeping.

STATIC const uint16_t days_since_jan1[]= { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

STATIC bool is_leap_year(mp_uint_t year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

// compute the day of the year, between 1 and 366
// month should be between 1 and 12, date should start at 1
mp_uint_t mod_time_year_day(mp_uint_t year, mp_uint_t month, mp_uint_t date) {
    mp_uint_t yday = days_since_jan1[month - 1] + date;
    if (month >= 3 && is_leap_year(year)) {
        yday += 1;
    }
    return yday;
}

// returns the number of seconds, as an integer, since 1/1/2000
mp_uint_t mod_time_seconds_since_2000(mp_uint_t year, mp_uint_t month, mp_uint_t date, mp_uint_t hour, mp_uint_t minute, mp_uint_t second) {
    return
        second
        + minute * 60
        + hour * 3600
        + (mod_time_year_day(year, month, date) - 1
            + ((year - 2000 + 3) / 4) // add a day each 4 years starting with 2001
            - ((year - 2000 + 99) / 100) // subtract a day each 100 years starting with 2001
            + ((year - 2000 + 399) / 400) // add a day each 400 years starting with 2001
            ) * 86400
        + (year - 2000) * 31536000;
}

/// \function localtime()
/// Returns time stored in RTC as: (year, month, date, hour, minute, second, weekday).
/// Weekday is 0-6 for Mon-Sun.
STATIC mp_obj_t time_localtime(void) {
    // get date and time
    // note: need to call get time then get date to correctly access the registers
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&RTCHandle, &time, FORMAT_BIN);
    HAL_RTC_GetDate(&RTCHandle, &date, FORMAT_BIN);
    mp_obj_t tuple[8] = {
        mp_obj_new_int(2000 + date.Year),
        mp_obj_new_int(date.Month),
        mp_obj_new_int(date.Date),
        mp_obj_new_int(time.Hours),
        mp_obj_new_int(time.Minutes),
        mp_obj_new_int(time.Seconds),
        mp_obj_new_int(date.WeekDay - 1),
        mp_obj_new_int(mod_time_year_day(2000 + date.Year, date.Month, date.Date)),
    };
    return mp_obj_new_tuple(8, tuple);
}
MP_DEFINE_CONST_FUN_OBJ_0(time_localtime_obj, time_localtime);

/// \function sleep(seconds)
/// Sleep for the given number of seconds.  Seconds can be a floating-point number to
/// sleep for a fractional number of seconds.
STATIC mp_obj_t time_sleep(mp_obj_t seconds_o) {
#if MICROPY_PY_BUILTINS_FLOAT
    if (MP_OBJ_IS_INT(seconds_o)) {
#endif
        HAL_Delay(1000 * mp_obj_get_int(seconds_o));
#if MICROPY_PY_BUILTINS_FLOAT
    } else {
        HAL_Delay((uint32_t)(1000 * mp_obj_get_float(seconds_o)));
    }
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

/// \function time()
/// Returns the number of seconds, as an integer, since 1/1/2000.
STATIC mp_obj_t time_time(void) {
    // get date and time
    // note: need to call get time then get date to correctly access the registers
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&RTCHandle, &time, FORMAT_BIN);
    HAL_RTC_GetDate(&RTCHandle, &date, FORMAT_BIN);
    return mp_obj_new_int(mod_time_seconds_since_2000(2000 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds));
}
MP_DEFINE_CONST_FUN_OBJ_0(time_time_obj, time_time);

STATIC const mp_map_elem_t time_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_time) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_localtime), (mp_obj_t)&time_localtime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep), (mp_obj_t)&time_sleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t)&time_time_obj },
};

STATIC const mp_obj_dict_t time_module_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(time_module_globals_table),
        .alloc = MP_ARRAY_SIZE(time_module_globals_table),
        .table = (mp_map_elem_t*)time_module_globals_table,
    },
};

const mp_obj_module_t time_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_time,
    .globals = (mp_obj_dict_t*)&time_module_globals,
};
