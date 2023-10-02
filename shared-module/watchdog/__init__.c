/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 MicroDev
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

#include "py/runtime.h"

#include "shared/runtime/pyexec.h"

#include "shared-module/watchdog/__init__.h"

#include "shared-bindings/watchdog/WatchDogTimer.h"
#include "shared-bindings/microcontroller/__init__.h"

void watchdog_reset(void) {
    watchdog_watchdogtimer_obj_t *self = &common_hal_mcu_watchdogtimer_obj;
    if (self->mode == WATCHDOGMODE_RESET) {
        mp_obj_t exception = pyexec_result()->exception;
        if (exception != MP_OBJ_NULL &&
            exception != MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception)) &&
            exception != MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_reload_exception))) {
            return;
        }
    }
    common_hal_watchdog_deinit(self);
}
