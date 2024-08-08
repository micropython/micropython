// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2020 microDev
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/rtc/RTC.h"
#include "common-hal/rtc/RTC.h"

#include <sys/time.h>

#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"

#include "src/common/pico_util/include/pico/util/datetime.h"
#include "src/rp2_common/pico_aon_timer/include/pico/aon_timer.h"

void common_hal_rtc_init(void) {
    // We start the RTC at 0 which mark as January 1, 2000.
    struct timespec t = {
        .tv_sec = 0,
        .tv_nsec = 0
    };
    aon_timer_start(&t);
}

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    struct timespec t;
    aon_timer_get_time(&t);

    timeutils_seconds_since_2000_to_struct_time(t.tv_sec, tm);
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    struct timespec t;
    t.tv_nsec = 0;
    t.tv_sec = timeutils_seconds_since_2000(tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    aon_timer_set_time(&t);
}

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
}
