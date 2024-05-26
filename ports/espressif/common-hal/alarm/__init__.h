// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries.
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/alarm/SleepMemory.h"
#include "common-hal/alarm/pin/PinAlarm.h"
#include "common-hal/alarm/time/TimeAlarm.h"
#include "common-hal/alarm/touch/TouchAlarm.h"

#if CIRCUITPY_ESPULP
#include "common-hal/espulp/ULPAlarm.h"
#endif

typedef union {
    #if CIRCUITPY_ESPULP
    espulp_ulpalarm_obj_t ulp_alarm;
    #endif
    alarm_pin_pinalarm_obj_t pin_alarm;
    alarm_time_timealarm_obj_t time_alarm;
    alarm_touch_touchalarm_obj_t touch_alarm;
} alarm_wake_alarm_union_t;

extern alarm_wake_alarm_union_t alarm_wake_alarm;
extern const alarm_sleep_memory_obj_t alarm_sleep_memory_obj;

extern void alarm_reset(void);
