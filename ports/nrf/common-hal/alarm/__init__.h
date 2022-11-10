/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries.
 * Copyright (c) 2021 Junji Sakai
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
#include "common-hal/alarm/touch/TouchAlarm.h"

typedef enum {
    NRF_SLEEP_WAKEUP_UNDEFINED,
    NRF_SLEEP_WAKEUP_GPIO,
    NRF_SLEEP_WAKEUP_TIMER,
    NRF_SLEEP_WAKEUP_TOUCHPAD,
    NRF_SLEEP_WAKEUP_VBUS,
    NRF_SLEEP_WAKEUP_RESETPIN,
    NRF_SLEEP_WAKEUP_ZZZ
} nrf_sleep_source_t;

typedef union {
    alarm_pin_pinalarm_obj_t pin_alarm;
    alarm_time_timealarm_obj_t time_alarm;
    alarm_touch_touchalarm_obj_t touch_alarm;
} alarm_wake_alarm_union_t;

extern alarm_wake_alarm_union_t alarm_wake_alarm;
extern const alarm_sleep_memory_obj_t alarm_sleep_memory_obj;

enum {
    SLEEPMEM_WAKEUP_BY_NONE  = 0,
    SLEEPMEM_WAKEUP_BY_PIN   = 1,
    SLEEPMEM_WAKEUP_BY_TIMER = 2,
};
#define WAKEUP_PIN_UNDEF 0xFF
extern uint8_t sleepmem_wakeup_event;
extern uint8_t sleepmem_wakeup_pin;

extern void alarm_reset(void);
