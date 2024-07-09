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
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/rtc/RTC.h"
#include "common-hal/rtc/RTC.h"

#include "sdk/drivers/snvs_hp/fsl_snvs_hp.h"
#include "sdk/drivers/snvs_lp/fsl_snvs_lp.h"

void rtc_init(void) {
    snvs_hp_rtc_config_t hpconfig;
    SNVS_HP_RTC_GetDefaultConfig(&hpconfig);

    SNVS_HP_RTC_Init(SNVS, &hpconfig);

    snvs_lp_srtc_config_t lpconfig;
    SNVS_LP_SRTC_GetDefaultConfig(&lpconfig);

    SNVS_LP_SRTC_Init(SNVS, &lpconfig);

    SNVS_LP_SRTC_StartTimer(SNVS);
    SNVS_HP_RTC_StartTimer(SNVS);
}

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    snvs_lp_srtc_datetime_t rtcDate;
    SNVS_LP_SRTC_GetDatetime(SNVS, &rtcDate);

    tm->tm_year = rtcDate.year;
    tm->tm_mon = rtcDate.month;
    tm->tm_mday = rtcDate.day;
    tm->tm_hour = rtcDate.hour;
    tm->tm_min = rtcDate.minute;
    tm->tm_sec = rtcDate.second;
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    snvs_lp_srtc_datetime_t rtcDate;
    rtcDate.year = tm->tm_year;
    rtcDate.month = tm->tm_mon;
    rtcDate.day = tm->tm_mday;
    rtcDate.hour = tm->tm_hour;
    rtcDate.minute = tm->tm_min;
    rtcDate.second = tm->tm_sec;

    SNVS_LP_SRTC_SetDatetime(SNVS, &rtcDate);
}

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    // SNVS has HPCALB_VAL bits for calibration.
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
}
