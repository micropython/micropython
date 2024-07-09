// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/shared/lock.h"

void supervisor_acquire_lock(supervisor_lock_t *lock) {
    while (!supervisor_try_lock(lock)) {
        RUN_BACKGROUND_TASKS;
    }
}

bool supervisor_try_lock(supervisor_lock_t *lock) {
    bool grabbed_lock = false;
    common_hal_mcu_disable_interrupts();
    if (!*lock) {
        *lock = true;
        grabbed_lock = true;
    }
    common_hal_mcu_enable_interrupts();
    return grabbed_lock;
}

void supervisor_release_lock(supervisor_lock_t *lock) {
    common_hal_mcu_disable_interrupts();
    *lock = false;
    common_hal_mcu_enable_interrupts();
}
