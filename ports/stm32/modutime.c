/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
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
#include "rtc.h"

// Return the localtime as an 8-tuple.
STATIC mp_obj_t mp_utime_localtime_get(void) {
    // get current date and time
    // note: need to call get time then get date to correctly access the registers
    rtc_init_finalise();
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
    mp_obj_t tuple[8] = {
        mp_obj_new_int(2000 + date.Year),
        mp_obj_new_int(date.Month),
        mp_obj_new_int(date.Date),
        mp_obj_new_int(time.Hours),
        mp_obj_new_int(time.Minutes),
        mp_obj_new_int(time.Seconds),
        mp_obj_new_int(date.WeekDay - 1),
        mp_obj_new_int(timeutils_year_day(2000 + date.Year, date.Month, date.Date)),
    };
    return mp_obj_new_tuple(8, tuple);
}

// Returns the number of seconds, as an integer, since 1/1/2000.
STATIC mp_obj_t mp_utime_time_get(void) {
    // get date and time
    // note: need to call get time then get date to correctly access the registers
    rtc_init_finalise();
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&RTCHandle, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTCHandle, &date, RTC_FORMAT_BIN);
    return mp_obj_new_int(timeutils_seconds_since_epoch(2000 + date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds));
}
