/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2023 Damien P. George
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
#include "modmachine.h"

static uint64_t time_us_64_offset_from_epoch;

// Return the localtime as an 8-tuple.
static mp_obj_t mp_time_localtime_get(void) {
    timeutils_struct_time_t tm;
    rtc_gettime(&tm);
    tm.tm_wday = timeutils_calc_weekday(tm.tm_year, tm.tm_mon, tm.tm_mday);
    tm.tm_yday = timeutils_year_day(tm.tm_year, tm.tm_mon, tm.tm_mday);
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

// Returns the number of seconds, as an integer, since the Epoch.
static mp_obj_t mp_time_time_get(void) {
    timeutils_struct_time_t tm;
    rtc_gettime(&tm);
    return mp_obj_new_int_from_uint(timeutils_mktime(
        tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec));
}

void mp_hal_time_ns_set_from_rtc(void) {
    timeutils_struct_time_t tm;
    rtc_gettime(&tm);
    uint64_t time_us = (uint64_t)timeutils_mktime(tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour,
        tm.tm_min, tm.tm_sec) * 1000000ULL;
    time_us_64_offset_from_epoch = time_us - mp_hal_ticks_us_64();
}

uint64_t mp_hal_time_ns(void) {
    return (time_us_64_offset_from_epoch + mp_hal_ticks_us_64()) * 1000ULL;
}
