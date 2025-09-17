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

#include "py/obj.h"

#include "shared/timeutils/timeutils.h"

// To maintain reasonable compatibility with CPython on embedded systems,
// and avoid breaking anytime soon, timeutils functions are required to
// work properly between 1970 and 2099 on all ports.
//
// During that period of time, leap years occur every 4 years without
// exception, so we can keep the code short for 32 bit machines.

// The last leap day before the required period is Feb 29, 1968.
// This is the number of days to add to get to that date.
#define PREV_LEAP_DAY  ((mp_uint_t)(365 + 366 - (31 + 29)))
#define PREV_LEAP_YEAR 1968

// On ports where either MICROPY_TIME_SUPPORT_Y2100_AND_BEYOND or
// MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE is enabled, we include extra
// code to support leap years outside of the 'easy' period.
// Computation is then made based on 1600 (a mod-400 year).
// This is the number of days between 1600 and 1968.
#define QC_BASE_DAY  134409
#define QC_LEAP_YEAR 1600
// This is the number of leap days between 1600 and 1970
#define QC_LEAP_DAYS 89

#define DAYS_PER_400Y (365 * 400 + 97)
#define DAYS_PER_100Y (365 * 100 + 24)
#define DAYS_PER_4Y   (365 * 4 + 1)

static const uint16_t days_since_jan1[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

// type used internally to count small integers relative to epoch
// (using uint when possible produces smaller code on some platforms)
#if MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
typedef mp_int_t relint_t;
#else
typedef mp_uint_t relint_t;
#endif

bool timeutils_is_leap_year(mp_uint_t year) {
    #if MICROPY_TIME_SUPPORT_Y2100_AND_BEYOND || MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
    #else
    return year % 4 == 0;
    #endif
}

// month is one based
mp_uint_t timeutils_days_in_month(mp_uint_t year, mp_uint_t month) {
    mp_uint_t mdays = days_since_jan1[month] - days_since_jan1[month - 1];
    if (month == 2 && timeutils_is_leap_year(year)) {
        mdays++;
    }
    return mdays;
}

// compute the day of the year, between 1 and 366
// month should be between 1 and 12, date should start at 1
mp_uint_t timeutils_year_day(mp_uint_t year, mp_uint_t month, mp_uint_t date) {
    mp_uint_t yday = days_since_jan1[month - 1] + date;
    if (month >= 3 && timeutils_is_leap_year(year)) {
        yday += 1;
    }
    return yday;
}

void timeutils_seconds_since_1970_to_struct_time(timeutils_timestamp_t seconds, timeutils_struct_time_t *tm) {
    // The following algorithm was inspired from musl's __secs_to_tm
    // and simplified to reduce code footprint in the simple case

    relint_t days = seconds / 86400;
    seconds %= 86400;
    #if MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
    if (seconds < 0) {
        seconds += 86400;
        days -= 1;
    }
    #endif
    tm->tm_hour = seconds / 3600;
    tm->tm_min = seconds / 60 % 60;
    tm->tm_sec = seconds % 60;

    relint_t wday = (days + 3) % 7;   // Jan 1, 1970 was a Thursday (3)
    #if MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
    if (wday < 0) {
        wday += 7;
    }
    #endif
    tm->tm_wday = wday;

    days += PREV_LEAP_DAY;

    #if MICROPY_TIME_SUPPORT_Y2100_AND_BEYOND || MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
    // rebase day to the oldest supported date (=> always positive)
    mp_uint_t base_year = QC_LEAP_YEAR;
    days += QC_BASE_DAY;
    mp_uint_t qc_cycles = days / DAYS_PER_400Y;
    days %= DAYS_PER_400Y;
    mp_uint_t c_cycles = days / DAYS_PER_100Y;
    if (c_cycles == 4) {
        c_cycles--;
    }
    days -= (c_cycles * DAYS_PER_100Y);
    #else
    mp_uint_t base_year = PREV_LEAP_YEAR;
    mp_uint_t qc_cycles = 0;
    mp_uint_t c_cycles = 0;
    #endif

    mp_uint_t q_cycles = days / DAYS_PER_4Y;
    #if MICROPY_TIME_SUPPORT_Y2100_AND_BEYOND || MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
    if (q_cycles == 25) {
        q_cycles--;
    }
    #endif
    days -= q_cycles * DAYS_PER_4Y;

    relint_t years = days / 365;
    if (years == 4) {
        years--;
    }
    days -= (years * 365);

    tm->tm_year = base_year + years + 4 * q_cycles + 100 * c_cycles + 400 * qc_cycles;

    // Note: days_in_month[0] corresponds to March
    static const uint8_t days_in_month[] = {31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29};

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

    tm->tm_yday = timeutils_year_day(tm->tm_year, tm->tm_mon, tm->tm_mday);
}

// returns the number of seconds, as an integer, since 2000-01-01
timeutils_timestamp_t timeutils_seconds_since_1970(mp_uint_t year, mp_uint_t month,
    mp_uint_t date, mp_uint_t hour, mp_uint_t minute, mp_uint_t second) {
    #if MICROPY_TIME_SUPPORT_Y2100_AND_BEYOND || MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
    mp_uint_t ref_year = QC_LEAP_YEAR;
    #else
    mp_uint_t ref_year = PREV_LEAP_YEAR;
    #endif
    timeutils_timestamp_t res;
    res = ((relint_t)year - 1970) * 365;
    res += (year - (ref_year + 1)) / 4; // add a day each 4 years
    #if MICROPY_TIME_SUPPORT_Y2100_AND_BEYOND || MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
    res -= (year - (ref_year + 1)) / 100; // subtract a day each 100 years
    res += (year - (ref_year + 1)) / 400; // add a day each 400 years
    res -= QC_LEAP_DAYS;
    #endif
    res += timeutils_year_day(year, month, date) - 1;
    res *= 86400;
    res += hour * 3600 + minute * 60 + second;
    return res;
}

timeutils_timestamp_t timeutils_mktime_1970(mp_uint_t year, mp_int_t month, mp_int_t mday,
    mp_int_t hours, mp_int_t minutes, mp_int_t seconds) {

    // Normalize the tuple. This allows things like:
    //
    // tm_tomorrow = list(time.localtime())
    // tm_tomorrow[2] += 1 # Adds 1 to mday
    // tomorrow = time.mktime(tm_tomorrow)
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
        mday += timeutils_days_in_month(year, month);
    }
    while ((mp_uint_t)mday > timeutils_days_in_month(year, month)) {
        mday -= timeutils_days_in_month(year, month);
        if (++month == 13) {
            month = 1;
            year++;
        }
    }
    return timeutils_seconds_since_1970(year, month, mday, hours, minutes, seconds);
}

// Calculate the weekday from the date.
// The result is zero based with 0 = Monday.
// by Michael Keith and Tom Craver, 1990.
int timeutils_calc_weekday(int y, int m, int d) {
    return ((d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4
        #if MICROPY_TIME_SUPPORT_Y2100_AND_BEYOND || MICROPY_TIME_SUPPORT_Y1969_AND_BEFORE
        - y / 100 + y / 400
        #endif
        ) + 6) % 7;
}
