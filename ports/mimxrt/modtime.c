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

// Return the localtime as an 8-tuple.
static mp_obj_t mp_time_localtime_get(void) {
    // Get current date and time.
    snvs_lp_srtc_datetime_t t;
    SNVS_LP_SRTC_GetDatetime(SNVS, &t);
    mp_obj_t tuple[8] = {
        mp_obj_new_int(t.year),
        mp_obj_new_int(t.month),
        mp_obj_new_int(t.day),
        mp_obj_new_int(t.hour),
        mp_obj_new_int(t.minute),
        mp_obj_new_int(t.second),
        mp_obj_new_int(timeutils_calc_weekday(t.year, t.month, t.day)),
        mp_obj_new_int(timeutils_year_day(t.year, t.month, t.day)),
    };
    return mp_obj_new_tuple(8, tuple);
}

// Return the number of seconds since the Epoch.
static mp_obj_t mp_time_time_get(void) {
    snvs_lp_srtc_datetime_t t;
    SNVS_LP_SRTC_GetDatetime(SNVS, &t);
    // EPOCH is 1970 for this port, which leads to the following trouble:
    // timeutils_seconds_since_epoch() calls timeutils_seconds_since_2000(), and
    // timeutils_seconds_since_2000() subtracts 2000 from year, but uses
    // an unsigned number for seconds, That causes an underrun, which is not
    // fixed by adding the TIMEUTILS_SECONDS_1970_TO_2000.
    // Masking it to 32 bit for year < 2000 fixes it.
    return mp_obj_new_int_from_ull(
        timeutils_seconds_since_epoch(t.year, t.month, t.day, t.hour, t.minute, t.second)
        & (t.year < 2000 ? 0xffffffff : 0xffffffffffff)
        );
}
