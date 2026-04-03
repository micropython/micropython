/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"

#include "helios_debug.h"
#include "helios_rtc.h"

uint64_t mp_time_start_second;

static void mp_time_localtime_get(timeutils_struct_time_t *tm) {
    Helios_RTCTime rtc_tm;
    Helios_RTC_GetLocalTime(&rtc_tm);
    tm->tm_year = rtc_tm.tm_year;
    tm->tm_mon = rtc_tm.tm_mon;
    tm->tm_mday = rtc_tm.tm_mday;
    tm->tm_hour = rtc_tm.tm_hour;
    tm->tm_min = rtc_tm.tm_min;
    tm->tm_sec = rtc_tm.tm_sec;
    tm->tm_wday = rtc_tm.tm_wday;
    tm->tm_yday = 0;
}

// return rtc seconds since power on
static mp_obj_t mp_time_time_get(void) {
    uint64_t seconds = Helios_RTC_GetSecond() - mp_time_start_second;
    return mp_obj_new_int_from_ull(seconds);
}

uint64_t mp_hal_time_ns(void) {
    return (Helios_RTC_GetSecond() - mp_time_start_second) * 1000000000ULL;
}
