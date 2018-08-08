/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#include <peripheral_clk_config.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>
#include <hal_calendar.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "lib/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "supervisor/shared/translate.h"

static struct calendar_descriptor calendar;

void rtc_init(void) {
#ifdef SAMD21
    _gclk_enable_channel(RTC_GCLK_ID, CONF_GCLK_RTC_SRC);
#endif
#ifdef SAMD51
    hri_mclk_set_APBAMASK_RTC_bit(MCLK);
#endif
    calendar_init(&calendar, RTC);
    calendar_set_baseyear(&calendar, 2000);
    calendar_enable(&calendar);
}

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    struct calendar_date_time datetime;
    calendar_get_date_time(&calendar, &datetime);

    tm->tm_year = datetime.date.year;
    tm->tm_mon  = datetime.date.month;
    tm->tm_mday = datetime.date.day;
    tm->tm_hour = datetime.time.hour;
    tm->tm_min  = datetime.time.min;
    tm->tm_sec  = datetime.time.sec;
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    // Reset prescaler to increase initial precision. Otherwise we can be up to 1 second off already.
    uint32_t freqcorr = hri_rtcmode0_read_FREQCORR_reg(calendar.device.hw);
    calendar_deinit(&calendar);
    rtc_init();
    hri_rtcmode0_write_FREQCORR_reg(calendar.device.hw, freqcorr);

    struct calendar_date date = {
        .year = tm->tm_year,
        .month = tm->tm_mon,
        .day = tm->tm_mday,
    };
    calendar_set_date(&calendar, &date);

    struct calendar_time time = {
        .hour = tm->tm_hour,
        .min = tm->tm_min,
        .sec = tm->tm_sec,
    };
    calendar_set_time(&calendar, &time);
}

// A positive value speeds up the clock by removing clock cycles.
int common_hal_rtc_get_calibration(void) {
    int calibration = hri_rtcmode0_read_FREQCORR_VALUE_bf(calendar.device.hw);

    if (!hri_rtcmode0_get_FREQCORR_SIGN_bit(calendar.device.hw))
        calibration = -calibration;

    return calibration;
}

void common_hal_rtc_set_calibration(int calibration) {
    if (calibration > 127 || calibration < -127)
        mp_raise_ValueError(translate("calibration value out of range +/-127"));

    hri_rtcmode0_write_FREQCORR_SIGN_bit(calendar.device.hw, calibration < 0 ? 0 : 1);
    hri_rtcmode0_write_FREQCORR_VALUE_bf(calendar.device.hw, abs(calibration));
}
