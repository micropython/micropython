/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Nick Moore for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

#include "fsl_snvs_hp.h"

void rtc_init(void) {
    snvs_hp_rtc_config_t config;
    SNVS_HP_RTC_GetDefaultConfig(&config);

    SNVS_HP_RTC_Init(SNVS, &config);
    SNVS_HP_RTC_StartTimer(SNVS);
}

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    snvs_hp_rtc_datetime_t rtcDate;
    SNVS_HP_RTC_GetDatetime(SNVS, &rtcDate);

    tm->tm_year = rtcDate.year;
    tm->tm_mon = rtcDate.month;
    tm->tm_mday = rtcDate.day;
    tm->tm_hour = rtcDate.hour;
    tm->tm_min = rtcDate.minute;
    tm->tm_sec = rtcDate.second;
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    snvs_hp_rtc_datetime_t rtcDate;
    rtcDate.year = tm->tm_year;
    rtcDate.month = tm->tm_mon;
    rtcDate.day = tm->tm_mday;
    rtcDate.hour = tm->tm_hour;
    rtcDate.minute = tm->tm_min;
    rtcDate.second = tm->tm_sec;

    SNVS_HP_RTC_SetDatetime(SNVS, &rtcDate);
}

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    // SNVS has HPCALB_VAL bits for calibration.
    mp_raise_NotImplementedError(translate("RTC calibration is not supported on this board"));
}
