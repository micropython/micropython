// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Nick Moore for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
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

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
}
