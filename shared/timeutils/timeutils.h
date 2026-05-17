/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_LIB_TIMEUTILS_TIMEUTILS_H
#define MICROPY_INCLUDED_LIB_TIMEUTILS_TIMEUTILS_H

#include "py/obj.h"
#if MICROPY_PY_BUILTINS_FLOAT && MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#include <math.h> // required for trunc()
#endif

// `timeutils_timestamp_t` is the type used internally by timeutils to
// represent timestamps, and is always referenced to 1970.
// It may not match the platform-specific `mp_timestamp_t`.
#if MICROPY_TIME_SUPPORT_Y2100_AND_BEYOND || MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
typedef long long timeutils_timestamp_t;
#else
typedef mp_uint_t timeutils_timestamp_t;
#endif

// The number of seconds between 1970/1/1 and 2000/1/1 is calculated using:
// time.mktime((2000,1,1,0,0,0,0,0,0)) - time.mktime((1970,1,1,0,0,0,0,0,0))
#define TIMEUTILS_SECONDS_1970_TO_2000 (946684800LL)

typedef struct _timeutils_struct_time_t {
    uint16_t tm_year;       // i.e. 2014
    uint8_t tm_mon;         // 1..12
    uint8_t tm_mday;        // 1..31
    uint8_t tm_hour;        // 0..23
    uint8_t tm_min;         // 0..59
    uint8_t tm_sec;         // 0..59
    uint8_t tm_wday;        // 0..6  0 = Monday
    uint16_t tm_yday;       // 1..366
} timeutils_struct_time_t;

bool timeutils_is_leap_year(mp_uint_t year);
mp_uint_t timeutils_days_in_month(mp_uint_t year, mp_uint_t month);
mp_uint_t timeutils_year_day(mp_uint_t year, mp_uint_t month, mp_uint_t date);
int timeutils_calc_weekday(int y, int m, int d);

void timeutils_seconds_since_1970_to_struct_time(timeutils_timestamp_t t,
    timeutils_struct_time_t *tm);

// Year is absolute, month/date are 1-based, hour/minute/second are 0-based.
timeutils_timestamp_t timeutils_seconds_since_1970(mp_uint_t year, mp_uint_t month,
    mp_uint_t date, mp_uint_t hour, mp_uint_t minute, mp_uint_t second);

// Year is absolute, month/mday are 1-based, hours/minutes/seconds are 0-based.
timeutils_timestamp_t timeutils_mktime_1970(mp_uint_t year, mp_int_t month, mp_int_t mday,
    mp_int_t hours, mp_int_t minutes, mp_int_t seconds);

static inline mp_timestamp_t timeutils_obj_get_timestamp(mp_obj_t o_in) {
    #if MICROPY_PY_BUILTINS_FLOAT && MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
    mp_float_t val = mp_obj_get_float(o_in);
    return (mp_timestamp_t)MICROPY_FLOAT_C_FUN(trunc)(val);
    #elif MICROPY_TIMESTAMP_IMPL == MICROPY_TIMESTAMP_IMPL_UINT
    return mp_obj_get_uint(o_in);
    #else
    return mp_obj_get_ll(o_in);
    #endif
}

static inline mp_obj_t timeutils_obj_from_timestamp(mp_timestamp_t t) {
    #if MICROPY_TIMESTAMP_IMPL == MICROPY_TIMESTAMP_IMPL_UINT
    return mp_obj_new_int_from_uint(t);
    #else
    return mp_obj_new_int_from_ll(t);
    #endif
}

static inline void timeutils_seconds_since_2000_to_struct_time(mp_timestamp_t t, timeutils_struct_time_t *tm) {
    timeutils_seconds_since_1970_to_struct_time((timeutils_timestamp_t)(t + TIMEUTILS_SECONDS_1970_TO_2000), tm);
}

// Year is absolute, month/date are 1-based, hour/minute/second are 0-based.
static inline mp_timestamp_t timeutils_seconds_since_2000(mp_uint_t year, mp_uint_t month, mp_uint_t date,
    mp_uint_t hour, mp_uint_t minute, mp_uint_t second) {
    return (mp_timestamp_t)timeutils_seconds_since_1970(year, month, date, hour, minute, second) - TIMEUTILS_SECONDS_1970_TO_2000;
}

// Year is absolute, month/mday are 1-based, hours/minutes/seconds are 0-based.
static inline mp_timestamp_t timeutils_mktime_2000(mp_uint_t year, mp_int_t month, mp_int_t mday,
    mp_int_t hours, mp_int_t minutes, mp_int_t seconds) {
    return (mp_timestamp_t)timeutils_mktime_1970(year, month, mday, hours, minutes, seconds) - TIMEUTILS_SECONDS_1970_TO_2000;
}


// Select the Epoch used by the port.
#if MICROPY_EPOCH_IS_1970

static inline void timeutils_seconds_since_epoch_to_struct_time(mp_timestamp_t t, timeutils_struct_time_t *tm) {
    timeutils_seconds_since_1970_to_struct_time(t, tm);
}

// Year is absolute, month/date are 1-based, hour/minute/second are 0-based.
static inline mp_timestamp_t timeutils_seconds_since_epoch(mp_uint_t year, mp_uint_t month, mp_uint_t date,
    mp_uint_t hour, mp_uint_t minute, mp_uint_t second) {
    return timeutils_seconds_since_1970(year, month, date, hour, minute, second);
}

// Year is absolute, month/mday are 1-based, hours/minutes/seconds are 0-based.
static inline mp_timestamp_t timeutils_mktime(mp_uint_t year, mp_int_t month, mp_int_t mday,
    mp_int_t hours, mp_int_t minutes, mp_int_t seconds) {
    return timeutils_mktime_1970(year, month, mday, hours, minutes, seconds);
}

static inline mp_timestamp_t timeutils_seconds_since_epoch_from_nanoseconds_since_1970(int64_t ns) {
    return (mp_timestamp_t)(ns / 1000000000LL);
}

static inline int64_t timeutils_seconds_since_epoch_to_nanoseconds_since_1970(mp_timestamp_t s) {
    return (int64_t)s * 1000000000LL;
}

static inline int64_t timeutils_nanoseconds_since_epoch_to_nanoseconds_since_1970(int64_t ns) {
    return ns;
}

#else // Epoch is 2000

static inline void timeutils_seconds_since_epoch_to_struct_time(mp_timestamp_t t, timeutils_struct_time_t *tm) {
    timeutils_seconds_since_2000_to_struct_time(t, tm);
}

// Year is absolute, month/date are 1-based, hour/minute/second are 0-based.
static inline mp_timestamp_t timeutils_seconds_since_epoch(mp_uint_t year, mp_uint_t month, mp_uint_t date,
    mp_uint_t hour, mp_uint_t minute, mp_uint_t second) {
    return timeutils_seconds_since_2000(year, month, date, hour, minute, second);
}

// Year is absolute, month/mday are 1-based, hours/minutes/seconds are 0-based.
static inline mp_timestamp_t timeutils_mktime(mp_uint_t year, mp_int_t month, mp_int_t mday,
    mp_int_t hours, mp_int_t minutes, mp_int_t seconds) {
    return timeutils_mktime_2000(year, month, mday, hours, minutes, seconds);
}

static inline mp_timestamp_t timeutils_seconds_since_epoch_from_nanoseconds_since_1970(int64_t ns) {
    return (mp_timestamp_t)(ns / 1000000000LL - TIMEUTILS_SECONDS_1970_TO_2000);
}

static inline int64_t timeutils_seconds_since_epoch_to_nanoseconds_since_1970(mp_timestamp_t s) {
    return ((int64_t)s + TIMEUTILS_SECONDS_1970_TO_2000) * 1000000000LL;
}

static inline int64_t timeutils_nanoseconds_since_epoch_to_nanoseconds_since_1970(int64_t ns) {
    return ns + TIMEUTILS_SECONDS_1970_TO_2000 * 1000000000LL;
}

#endif

#endif // MICROPY_INCLUDED_LIB_TIMEUTILS_TIMEUTILS_H
