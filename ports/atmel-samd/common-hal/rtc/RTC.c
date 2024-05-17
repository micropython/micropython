// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include <peripheral_clk_config.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/rtc/RTC.h"
#include "supervisor/port.h"

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
    mp_arg_validate_int_range(calibration, -127, 127, MP_QSTR_calibration);

    hri_rtcmode0_write_FREQCORR_SIGN_bit(RTC, calibration < 0 ? 0 : 1);
    hri_rtcmode0_write_FREQCORR_VALUE_bf(RTC, abs(calibration));
}
