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
static mp_obj_t mp_time_localtime_get(void) {
    // get current date and time
    rtc_init_finalise();
    ra_rtc_t time;
    ra_rtc_get_time(&time);
    mp_obj_t tuple[8] = {
        mp_obj_new_int(time.year),
        mp_obj_new_int(time.month),
        mp_obj_new_int(time.date),
        mp_obj_new_int(time.hour),
        mp_obj_new_int(time.minute),
        mp_obj_new_int(time.second),
        mp_obj_new_int(time.weekday - 1),
        mp_obj_new_int(timeutils_year_day(time.year, time.month, time.date)),
    };
    return mp_obj_new_tuple(8, tuple);
}

// Returns the number of seconds, as an integer, since the Epoch.
static mp_obj_t mp_time_time_get(void) {
    // get date and time
    rtc_init_finalise();
    ra_rtc_t time;
    ra_rtc_get_time(&time);
    return mp_obj_new_int(timeutils_seconds_since_epoch(time.year, time.month, time.date, time.hour, time.minute, time.second));
}
