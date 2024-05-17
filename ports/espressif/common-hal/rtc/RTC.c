// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include <sys/time.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "components/soc/include/soc/rtc_periph.h"
#include "shared-bindings/rtc/RTC.h"

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    timeutils_seconds_since_2000_to_struct_time(tv_now.tv_sec, tm);
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    struct timeval tv_now = {0};
    tv_now.tv_sec = timeutils_seconds_since_2000(tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    settimeofday(&tv_now, NULL);
}

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
}
