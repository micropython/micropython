/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbert for Adafruit Industries
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
