/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2023 Damien P. George
 * Copyright (c) 2020 Jim Mussared
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
#include "fsl_snvs_lp.h"

// Get the localtime.
static void mp_time_localtime_get(timeutils_struct_time_t *tm) {
    // Get current date and time.
    snvs_lp_srtc_datetime_t t;
    SNVS_LP_SRTC_GetDatetime(SNVS, &t);
    tm->tm_year = t.year;
    tm->tm_mon = t.month;
    tm->tm_mday = t.day;
    tm->tm_hour = t.hour;
    tm->tm_min = t.minute;
    tm->tm_sec = t.second;
    tm->tm_wday = timeutils_calc_weekday(t.year, t.month, t.day);
    tm->tm_yday = timeutils_year_day(t.year, t.month, t.day);
}

// Return the number of seconds since the Epoch.
static mp_obj_t mp_time_time_get(void) {
    snvs_lp_srtc_datetime_t t;
    SNVS_LP_SRTC_GetDatetime(SNVS, &t);
    return timeutils_obj_from_timestamp(
        timeutils_seconds_since_epoch(t.year, t.month, t.day, t.hour, t.minute, t.second)
        );
}
