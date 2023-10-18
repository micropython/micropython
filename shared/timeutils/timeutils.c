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

// LEAPOCH corresponds to 2000-03-01, which is a mod-400 year, immediately
// after Feb 29. We calculate seconds as a signed integer relative to that.
//
// Our timebase is relative to 2000-01-01.

#define LEAPOCH ((31 + 29) * 86400)

#define DAYS_PER_400Y (365 * 400 + 97)
#define DAYS_PER_100Y (365 * 100 + 24)
#define DAYS_PER_4Y   (365 * 4 + 1)

STATIC const uint16_t days_since_jan1[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

bool timeutils_is_leap_year(mp_uint_t year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
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

void timeutils_seconds_since_2000_to_struct_time(mp_uint_t t, timeutils_struct_time_t *tm) {
    // The following algorithm was adapted from musl's __secs_to_tm and adapted
    // for differences in MicroPython's timebase.

    mp_int_t seconds = t - LEAPOCH;

    mp_int_t days = seconds / 86400;
    seconds %= 86400;
    if (seconds < 0) {
        seconds += 86400;
        days -= 1;
    }
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

    /* We will compute tm_yday at the very end
    mp_int_t leap = !years && (q_cycles || !c_cycles);

    tm->tm_yday = days + 31 + 28 + leap;
    if (tm->tm_yday >= 365 + leap) {
        tm->tm_yday -= 365 + leap;
    }

    tm->tm_yday++;  // Make one based
    */

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

    tm->tm_yday = timeutils_year_day(tm->tm_year, tm->tm_mon, tm->tm_mday);
}

// returns the number of seconds, as an integer, since 2000-01-01
mp_uint_t timeutils_seconds_since_2000(mp_uint_t year, mp_uint_t month,
    mp_uint_t date, mp_uint_t hour, mp_uint_t minute, mp_uint_t second) {
    return
        second
        + minute * 60
        + hour * 3600
        + (timeutils_year_day(year, month, date) - 1
            + ((year - 2000 + 3) / 4) // add a day each 4 years starting with 2001
            - ((year - 2000 + 99) / 100) // subtract a day each 100 years starting with 2001
            + ((year - 2000 + 399) / 400) // add a day each 400 years starting with 2001
            ) * 86400
        + (year - 2000) * 31536000;
}

mp_uint_t timeutils_mktime_2000(mp_uint_t year, mp_int_t month, mp_int_t mday,
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
    return timeutils_seconds_since_2000(year, month, mday, hours, minutes, seconds);
}

// Calculate the weekday from the date.
// The result is zero based with 0 = Monday.
// by Michael Keith and Tom Craver, 1990.
int timeutils_calc_weekday(int y, int m, int d) {
    return ((d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) + 6) % 7;
}
