// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/alarm/SleepMemory.h"
#include "common-hal/alarm/pin/PinAlarm.h"
#include "common-hal/alarm/time/TimeAlarm.h"

extern const alarm_sleep_memory_obj_t alarm_sleep_memory_obj;

typedef enum {
    STM_WAKEUP_UNDEF,
    STM_WAKEUP_GPIO,
    STM_WAKEUP_RTC
} stm_sleep_source_t;

typedef union {
    alarm_pin_pinalarm_obj_t pin_alarm;
    alarm_time_timealarm_obj_t time_alarm;
} alarm_wake_alarm_union_t;

extern alarm_wake_alarm_union_t alarm_wake_alarm;

#define STM_ALARM_FLAG      (RTC->BKP0R)

extern void alarm_set_wakeup_reason(stm_sleep_source_t reason);
extern stm_sleep_source_t alarm_get_wakeup_cause(void);
extern void alarm_reset(void);
