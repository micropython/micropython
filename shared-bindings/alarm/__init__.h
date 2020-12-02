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

extern mp_obj_t common_hal_alarm_wait_until_alarms(size_t n_alarms, const mp_obj_t *alarms);
extern mp_obj_t common_hal_alarm_light_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms);
extern void common_hal_alarm_exit_and_deep_sleep_until_alarms(size_t n_alarms, const mp_obj_t *alarms);
extern void common_hal_alarm_prepare_for_deep_sleep(void);
extern NORETURN void common_hal_alarm_enter_deep_sleep(void);

// Used by wake-up code.
extern void common_hal_alarm_set_wake_alarm(mp_obj_t alarm);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_ALARM___INIT___H
