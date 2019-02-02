/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nick Moore for Adafruit Industries
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

#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "lib/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "supervisor/shared/translate.h"

void rtc_init(void) {
}

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    tm->tm_year = 2000;
    tm->tm_mon  = 1;
    tm->tm_mday = 2;
    tm->tm_hour = 3;
    tm->tm_min  = 4;
    tm->tm_sec  = 5;
    tm->tm_wday = 6;
    tm->tm_yday = 2;
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
}

// A positive value speeds up the clock by removing clock cycles.
int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
}
