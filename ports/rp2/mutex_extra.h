/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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
#ifndef MICROPY_INCLUDED_RP2_MUTEX_EXTRA_H
#define MICROPY_INCLUDED_RP2_MUTEX_EXTRA_H

#include "pico/mutex.h"

uint32_t recursive_mutex_enter_blocking_and_disable_interrupts(recursive_mutex_t *mtx);
void recursive_mutex_exit_and_restore_interrupts(recursive_mutex_t *mtx, uint32_t save);

// Alternative version of recursive_mutex_t that doesn't issue WFE and SEV
// instructions. This means it will use more power (busy-waits), but exiting
// this mutex doesn't disrupt the calling CPU's event state in the same way a
// recursive mutex does (because recurse_mutex_exit() executes SEV each time the
// mutex is released.)
//
// Implement as a wrapper type because no longer compatible with the normal
// recursive_mutex functions.

typedef struct {
    recursive_mutex_t mutex;
} recursive_mutex_nowait_t;

static inline void recursive_mutex_nowait_init(recursive_mutex_nowait_t *mtx) {
    recursive_mutex_init(&mtx->mutex);
}

static inline bool recursive_mutex_nowait_try_enter(recursive_mutex_nowait_t *mtx, uint32_t *owner_out) {
    return recursive_mutex_try_enter(&mtx->mutex, owner_out);
}

void recursive_mutex_nowait_enter_blocking(recursive_mutex_nowait_t *mtx);
void recursive_mutex_nowait_exit(recursive_mutex_nowait_t *mtx);

#endif // MICROPY_INCLUDED_RP2_MUTEX_EXTRA_H
