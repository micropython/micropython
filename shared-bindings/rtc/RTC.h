// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "py/obj.h"
#include "shared/timeutils/timeutils.h"

extern void common_hal_rtc_get_time(timeutils_struct_time_t *tm);
extern void common_hal_rtc_set_time(timeutils_struct_time_t *tm);

extern int common_hal_rtc_get_calibration(void);
extern void common_hal_rtc_set_calibration(int calibration);

extern const mp_obj_type_t rtc_rtc_type;

typedef struct _rtc_rtc_obj_t {
    mp_obj_base_t base;
} rtc_rtc_obj_t;

extern const rtc_rtc_obj_t rtc_rtc_obj;
