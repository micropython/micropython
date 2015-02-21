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
#include MICROPY_HAL_H
#include "py/nlr.h"
#include "py/obj.h"
#include "modutime.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"
#include "pybrtc.h"
#include "mpexception.h"


// LEAPOCH corresponds to 2000-03-01, which is a mod-400 year, immediately
// after Feb 29. We calculate seconds as a signed integer relative to that.
//
// Our timebase is relative to 2000-01-01.

#define LEAPOCH ((31 + 29) * 86400)

#define DAYS_PER_400Y (365*400 + 97)
#define DAYS_PER_100Y (365*100 + 24)
#define DAYS_PER_4Y   (365*4   + 1)


/// \module time - time related functions
///
/// The `time` module provides functions for getting the current time and date,
/// and for sleeping.

STATIC const uint16_t days_since_jan1[]= { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

STATIC bool is_leap_year(mp_uint_t year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

// Month is one based
STATIC mp_uint_t mod_time_days_in_month(mp_uint_t year, mp_uint_t month) {
    mp_uint_t mdays = days_since_jan1[month] - days_since_jan1[month - 1];
    if (month == 2 && is_leap_year(year)) {
        mdays++;
    }
    return mdays;
}

// compute the day of the year, between 1 and 366
// month should be between 1 and 12, date should start at 1
STATIC mp_uint_t mod_time_year_day(mp_uint_t year, mp_uint_t month, mp_uint_t date) {
    mp_uint_t yday = days_since_jan1[month - 1] + date;
    if (month >= 3 && is_leap_year(year)) {
        yday += 1;
    }
    return yday;
}

// returns the number of seconds, as an integer, since 2000-01-01
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

void mod_time_seconds_since_2000_to_struct_time(mp_uint_t t, mod_struct_time *tm) {
    // The following algorithm was adapted from musl's __secs_to_tm and adapted
    // for differences in Micro Python's timebase.

    mp_int_t seconds = t - LEAPOCH;

    mp_int_t days = seconds / 86400;
    seconds %= 86400;
    tm->tm_hour = seconds / 3600;
    tm->tm_min = seconds / 60 % 60;
    tm->tm_sec = seconds % 60;

    mp_int_t wday = (days + 2) % 7;   // Mar 1, 2000 was a Wednesday (2)
    if (wday < 0) {
        wday += 7;
    }
    tm->tm_wday = wday;

    mp_int_t qc_cycles = days / DAYS_PER_400Y;
    days %= DAYS_PER_400Y;
    if (days < 0) {
        days += DAYS_PER_400Y;
        qc_cycles--;
    }
    mp_int_t c_cycles = days / DAYS_PER_100Y;
    if (c_cycles == 4) {
        c_cycles--;
    }
    days -= (c_cycles * DAYS_PER_100Y);

    mp_int_t q_cycles = days / DAYS_PER_4Y;
    if (q_cycles == 25) {
        q_cycles--;
    }
    days -= q_cycles * DAYS_PER_4Y;

    mp_int_t years = days / 365;
    if (years == 4) {
        years--;
    }
    days -= (years * 365);

    tm->tm_year = 2000 + years + 4 * q_cycles + 100 * c_cycles + 400 * qc_cycles;

    // Note: days_in_month[0] corresponds to March
    STATIC const int8_t days_in_month[] = {31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29};

    mp_int_t month;
    for (month = 0; days_in_month[month] <= days; month++) {
        days -= days_in_month[month];
    }

    tm->tm_mon = month + 2;
    if (tm->tm_mon >= 12) {
        tm->tm_mon -= 12;
        tm->tm_year++;
    }
    tm->tm_mday = days + 1; // Make one based
    tm->tm_mon++;   // Make one based

    tm->tm_yday = mod_time_year_day(tm->tm_year, tm->tm_mon, tm->tm_mday);
}

/// \function localtime([secs])
/// Convert a time expressed in seconds since Jan 1, 2000 into an 8-tuple which
/// contains: (year, month, mday, hour, minute, second, weekday, yearday)
/// If secs is not provided or None, then the current time from the RTC is used.
/// year includes the century (for example 2014)
/// month   is 1-12
/// mday    is 1-31
/// hour    is 0-23
/// minute  is 0-59
/// second  is 0-59
/// weekday is 0-6 for Mon-Sun.
/// yearday is 1-366
STATIC mp_obj_t time_localtime(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 0 || args[0] == mp_const_none) {
        mod_struct_time tm;
        uint32_t seconds;
        uint16_t mseconds;

        // get the seconds and the milliseconds from the RTC
        MAP_PRCMRTCGet(&seconds, &mseconds);
        mseconds = RTC_CYCLES_U16MS(mseconds);
        mod_time_seconds_since_2000_to_struct_time(seconds, &tm);

        mp_obj_t tuple[8] = {
                mp_obj_new_int(tm.tm_year),
                mp_obj_new_int(tm.tm_mon),
                mp_obj_new_int(tm.tm_mday),
                mp_obj_new_int(tm.tm_wday),
                mp_obj_new_int(tm.tm_hour),
                mp_obj_new_int(tm.tm_min),
                mp_obj_new_int(tm.tm_sec),
                mp_obj_new_int(mseconds)
        };
        return mp_obj_new_tuple(8, tuple);
    } else {
        mp_int_t seconds = mp_obj_get_int(args[0]);
        mod_struct_time tm;
        mod_time_seconds_since_2000_to_struct_time(seconds, &tm);
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


/// \function mktime()
/// This is inverse function of localtime. It's argument is a full 8-tuple
/// which expresses a time as per localtime. It returns an integer which is
/// the number of seconds since Jan 1, 2000.
STATIC mp_obj_t time_mktime(mp_obj_t tuple) {

    mp_uint_t len;
    mp_obj_t *elem;

    mp_obj_get_array(tuple, &len, &elem);

    // localtime generates a tuple of len 8. CPython uses 9, so we accept both.
    if (len < 8 || len > 9) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, mpexception_num_type_invalid_arguments));
    }

    mp_int_t year    = mp_obj_get_int(elem[0]);
    mp_int_t month   = mp_obj_get_int(elem[1]);
    mp_int_t mday    = mp_obj_get_int(elem[2]);
    mp_int_t hours   = mp_obj_get_int(elem[3]);
    mp_int_t minutes = mp_obj_get_int(elem[4]);
    mp_int_t seconds = mp_obj_get_int(elem[5]);

    // Normalize the tuple. This allows things like:
    //
    // tm_tomorrow = list(time.localtime())
    // tm_tomorrow[2] += 1 # Adds 1 to mday
    // tomorrow = time.mktime(tm_tommorrow)
    //
    // And not have to worry about all the weird overflows.
    //
    // You can subtract dates/times this way as well.

    minutes += seconds / 60;
    if ((seconds = seconds % 60) < 0) {
        seconds += 60;
        minutes--;
    }

    hours += minutes / 60;
    if ((minutes = minutes % 60) < 0) {
        minutes += 60;
        hours--;
    }

    mday += hours / 24;
    if ((hours = hours % 24) < 0) {
        hours += 24;
        mday--;
    }

    month--; // make month zero based
    year += month / 12;
    if ((month = month % 12) < 0) {
        month += 12;
        year--;
    }
    month++; // back to one based

    while (mday < 1) {
        if (--month == 0) {
            month = 12;
            year--;
        }
        mday += mod_time_days_in_month(year, month);
    }
    while (mday > mod_time_days_in_month(year, month)) {
        mday -= mod_time_days_in_month(year, month);
        if (++month == 13) {
            month = 1;
            year++;
        }
    }
    return mp_obj_new_int_from_uint(mod_time_seconds_since_2000(year, month, mday, hours, minutes, seconds));
}
MP_DEFINE_CONST_FUN_OBJ_1(time_mktime_obj, time_mktime);


/// \function sleep(milliseconds)
/// Sleep for the given number of milliseconds.
STATIC mp_obj_t time_sleep(mp_obj_t milliseconds_o) {
    HAL_Delay(mp_obj_get_int(milliseconds_o));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(time_sleep_obj, time_sleep);

/// \function time()
/// Returns the number of seconds, as an integer, since 1/1/2000.
STATIC mp_obj_t time_time(void) {
    uint32_t seconds;
    uint16_t mseconds;

    // get the seconds and the milliseconds from the RTC
    MAP_PRCMRTCGet(&seconds, &mseconds);
    return mp_obj_new_int(seconds);
}
MP_DEFINE_CONST_FUN_OBJ_0(time_time_obj, time_time);

STATIC const mp_map_elem_t time_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_utime) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_localtime), (mp_obj_t)&time_localtime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mktime), (mp_obj_t)&time_mktime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep), (mp_obj_t)&time_sleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t)&time_time_obj },
};

STATIC MP_DEFINE_CONST_DICT(time_module_globals, time_module_globals_table);

const mp_obj_module_t mp_module_utime = {
    .base = { &mp_type_module },
    .name = MP_QSTR_utime,
    .globals = (mp_obj_dict_t*)&time_module_globals,
};
