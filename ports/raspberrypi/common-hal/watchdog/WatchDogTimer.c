// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/watchdog/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "common-hal/watchdog/WatchDogTimer.h"

#include "hardware/watchdog.h"

void common_hal_watchdog_feed(watchdog_watchdogtimer_obj_t *self) {
    watchdog_update();
}

void common_hal_watchdog_deinit(watchdog_watchdogtimer_obj_t *self) {
    if (self->mode == WATCHDOGMODE_NONE) {
        return;
    }
    hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
    self->mode = WATCHDOGMODE_NONE;
}

mp_float_t common_hal_watchdog_get_timeout(watchdog_watchdogtimer_obj_t *self) {
    return self->timeout;
}

void common_hal_watchdog_set_timeout(watchdog_watchdogtimer_obj_t *self, mp_float_t new_timeout) {
    // max timeout is 8.388607 sec, this is rounded down to 8 sec
    mp_arg_validate_int_max(new_timeout, 8, MP_QSTR_timeout);
    self->timeout = new_timeout;

    if (self->mode == WATCHDOGMODE_RESET) {
        watchdog_enable(self->timeout * 1000, false);
    }
}

watchdog_watchdogmode_t common_hal_watchdog_get_mode(watchdog_watchdogtimer_obj_t *self) {
    return self->mode;
}

void common_hal_watchdog_set_mode(watchdog_watchdogtimer_obj_t *self, watchdog_watchdogmode_t new_mode) {
    if (self->mode == new_mode) {
        return;
    }

    switch (new_mode) {
        case WATCHDOGMODE_NONE:
            common_hal_watchdog_deinit(self);
            break;
        case WATCHDOGMODE_RAISE:
            mp_raise_NotImplementedError(NULL);
            break;
        case WATCHDOGMODE_RESET:
            watchdog_enable(self->timeout * 1000, false);
            break;
        default:
            return;
    }

    self->mode = new_mode;
}
