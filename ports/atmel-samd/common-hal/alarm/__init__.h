/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Lucian Copeland for Adafruit Industries
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

#pragma once

#include "common-hal/alarm/SleepMemory.h"
#include "common-hal/alarm/pin/PinAlarm.h"
#include "common-hal/alarm/time/TimeAlarm.h"

extern const alarm_sleep_memory_obj_t alarm_sleep_memory_obj;

// This is the first byte of the BKUP register bank.
// We use it to store which alarms are set.
#ifndef SAMD_ALARM_FLAG
#define SAMD_ALARM_FLAG      (RTC->MODE0.BKUP[0].reg)
#define SAMD_ALARM_FLAG_TIME (_U_(0x1) << 0)
#define SAMD_ALARM_FLAG_PIN  (_U_(0x1) << 1)

#define SAMD_ALARM_FLAG_TIME_SET (SAMD_ALARM_FLAG |= SAMD_ALARM_FLAG_TIME)
#define SAMD_ALARM_FLAG_TIME_CLR (SAMD_ALARM_FLAG &= ~SAMD_ALARM_FLAG_TIME)
#define SAMD_ALARM_FLAG_TIME_CHK (SAMD_ALARM_FLAG & SAMD_ALARM_FLAG_TIME)

#define SAMD_ALARM_FLAG_PIN_SET (SAMD_ALARM_FLAG |= SAMD_ALARM_FLAG_PIN)
#define SAMD_ALARM_FLAG_PIN_CLR (SAMD_ALARM_FLAG &= ~SAMD_ALARM_FLAG_PIN)
#define SAMD_ALARM_FLAG_PIN_CHK (SAMD_ALARM_FLAG & SAMD_ALARM_FLAG_PIN)
#endif

typedef enum {
    SAMD_WAKEUP_UNDEF,
    SAMD_WAKEUP_GPIO,
    SAMD_WAKEUP_RTC
} samd_sleep_source_t;

typedef union {
    alarm_pin_pinalarm_obj_t pin_alarm;
    alarm_time_timealarm_obj_t time_alarm;
} alarm_wake_alarm_union_t;

extern alarm_wake_alarm_union_t alarm_wake_alarm;

extern void alarm_set_wakeup_reason(samd_sleep_source_t reason);
extern void alarm_get_wakeup_cause(void);
extern void alarm_reset(void);
