// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 20212 Matthew McGowan for Blues Wireless Inc
//
// SPDX-License-Identifier: MIT

#include <stdio.h>

#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "shared-bindings/rtc/RTC.h"
#include "peripherals/rtc.h"


void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    stm32_peripherals_rtc_set_time(tm);
}

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    stm32_peripherals_rtc_get_time(tm);
}

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
}
