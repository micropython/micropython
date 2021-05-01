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

#include "py/obj.h"
#include "py/runtime.h"
#include "lib/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "supervisor/port.h"
#include "supervisor/shared/translate.h"

// This is the time in seconds since 2000 that the RTC was started.
// TODO: Change the offset to ticks so that it can be a subsecond adjustment.
static uint32_t rtc_offset = 0;

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    uint64_t ticks_s = port_get_raw_ticks(NULL) / 1024;
    timeutils_seconds_since_2000_to_struct_time(rtc_offset + ticks_s, tm);
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    uint64_t ticks_s = port_get_raw_ticks(NULL) / 1024;
    uint32_t epoch_s = timeutils_seconds_since_2000(
        tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec
        );
    rtc_offset = epoch_s - ticks_s;
}

// A positive value speeds up the clock by removing clock cycles.
int common_hal_rtc_get_calibration(void) {
    int calibration = hri_rtcmode0_read_FREQCORR_VALUE_bf(RTC);

    if (!hri_rtcmode0_get_FREQCORR_SIGN_bit(RTC)) {
        calibration = -calibration;
    }

    return calibration;
}

void common_hal_rtc_set_calibration(int calibration) {
    if (calibration > 127 || calibration < -127) {
        #if CIRCUITPY_FULL_BUILD
        mp_raise_ValueError(translate("calibration value out of range +/-127"));
        #else
        mp_raise_ValueError(translate("calibration is out of range"));
        #endif
    }

    hri_rtcmode0_write_FREQCORR_SIGN_bit(RTC, calibration < 0 ? 0 : 1);
    hri_rtcmode0_write_FREQCORR_VALUE_bf(RTC, abs(calibration));
}
