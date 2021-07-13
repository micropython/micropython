/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H

#include "py/obj.h"

#include "common-hal/alarm/__init__.h"

// Light sleep fully self-contained and does not exit user code. It will return
// the same alarm object that was orignally passed in, unlike deep sleep, which
// must create an identical copy due to the VM reset
extern mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms);

// Deep sleep is a two step process. Alarms are set when the VM is valid but
// everything is reset before entering deep sleep. Furthermore, deep sleep may
// not actually happen if the user is connected to the device. In this case, the
// supervisor will idle using `port_wait_for_interrupt`. After each call, it will
// call alarm_woken_from_sleep to see if we've been woken by an alarm and if so,
// it will exit idle as if deep sleep was exited
extern void common_hal_alarm_set_deep_sleep_alarms(size_t n_alarms, const mp_obj_t *alarms);

extern NORETURN void common_hal_alarm_enter_deep_sleep(void);

// May be used to re-initialize peripherals like GPIO, if the VM reset returned
// them to a default state
extern void common_hal_alarm_pretending_deep_sleep(void);

// Fetches value from module dict.
extern mp_obj_t shared_alarm_get_wake_alarm(void);

// Creates a new alarm object after exiting deep sleep (real or fake)
extern mp_obj_t common_hal_alarm_create_wake_alarm(void);

// Saves alarm to global array
void shared_alarm_save_wake_alarm(mp_obj_t alarm);

// True if an alarm is alerting. Used in light sleep/fake deep sleep
extern bool common_hal_alarm_woken_from_sleep(void);

extern void common_hal_alarm_gc_collect(void);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H
