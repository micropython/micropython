// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

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
