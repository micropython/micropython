/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2020 microDev
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
#include "shared-bindings/rtc/RTC.h"

#include <sys/time.h>

#include "py/runtime.h"
#include "src/rp2_common/hardware_rtc/include/hardware/rtc.h"
#include "src/rp2_common/hardware_clocks/include/hardware/clocks.h"

void common_hal_rtc_init(void) {
    datetime_t t = {
        .year = 2020,
        .month = 1,
        .day = 1,
        .dotw = 3,      // 0 is Sunday, so 3 is Wednesday
        .hour = 0,
        .min = 0,
        .sec = 0
    };

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&t);

}

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    datetime_t t;
    rtc_get_datetime(&t);

    tm->tm_year = t.year;
    tm->tm_mon = t.month;
    tm->tm_mday = t.day;
    tm->tm_wday = t.dotw;
    tm->tm_hour = t.hour;
    tm->tm_min = t.min;
    tm->tm_sec = t.sec;

    if (tm->tm_wday == 0) {
        tm->tm_wday = 6;
    } else {
        tm->tm_wday -= 1;
    }
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    if (tm->tm_wday == 6) {
        tm->tm_wday = 0;
    } else {
        tm->tm_wday += 1;
    }

    datetime_t t = {
        .year = tm->tm_year,
        .month = tm->tm_mon,
        .day = tm->tm_mday,
        .dotw = tm->tm_wday,
        .hour = tm->tm_hour,
        .min = tm->tm_min,
        .sec = tm->tm_sec
    };
    rtc_set_datetime(&t);
}

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    mp_raise_NotImplementedError(translate("RTC calibration is not supported on this board"));
}
