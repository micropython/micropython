/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "py/obj.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "common-hal/rtc/RTC.h"
#include "shared-bindings/rtc/RTC.h"
#include "supervisor/port.h"
#include "supervisor/shared/translate/translate.h"
#include "peripherals/rtc.h"
#include "sl_sleeptimer.h"

// Set rtc time
void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {

    sl_sleeptimer_date_t date = { 0 };
    date.year = tm->tm_year - 1900;
    // Month enum begins at
    date.month = tm->tm_mon - 1;
    date.month_day = tm->tm_mday;
    date.day_of_week = tm->tm_wday == 6?0:tm->tm_wday + 1;
    date.hour = tm->tm_hour;
    date.min = tm->tm_min;
    date.sec = tm->tm_sec;
    date.day_of_year = tm->tm_yday;
    sl_sleeptimer_set_datetime(&date);
}

// Get rtc time
void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    sl_sleeptimer_date_t date;
    sl_sleeptimer_get_datetime(&date);

    tm->tm_year = date.year + 1900;
    tm->tm_mon = date.month + 1;
    tm->tm_mday = date.month_day;
    tm->tm_wday = date.day_of_week == 0?6:date.day_of_week - 1;
    tm->tm_hour = date.hour;
    tm->tm_min = date.min;
    tm->tm_sec = date.sec;
    tm->tm_yday = date.day_of_year;

}

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    mp_raise_NotImplementedError_varg(translate("%q"), MP_QSTR_calibration);
}
