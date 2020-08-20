/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_WATCHDOG_WATCHDOGTIMER_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_WATCHDOG_WATCHDOGTIMER_H

#include <py/obj.h>
#include "shared-bindings/watchdog/WatchDogMode.h"

typedef struct _watchdog_watchdogtimer_obj_t watchdog_watchdogtimer_obj_t;

extern void common_hal_watchdog_feed(watchdog_watchdogtimer_obj_t *self);

extern void common_hal_watchdog_set_mode(watchdog_watchdogtimer_obj_t *self, watchdog_watchdogmode_t);
extern watchdog_watchdogmode_t common_hal_watchdog_get_mode(watchdog_watchdogtimer_obj_t *self);

extern void common_hal_watchdog_set_timeout(watchdog_watchdogtimer_obj_t *self, mp_float_t timeout);
extern mp_float_t common_hal_watchdog_get_timeout(watchdog_watchdogtimer_obj_t *self);

extern void common_hal_watchdog_enable(watchdog_watchdogtimer_obj_t *self);
extern void common_hal_watchdog_deinit(watchdog_watchdogtimer_obj_t *self);

extern const mp_obj_type_t watchdog_watchdogtimer_type;

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_WATCHDOG_WATCHDOGTIMER_H
