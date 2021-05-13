/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SUPERVISOR_SAFE_MODE_H
#define MICROPY_INCLUDED_SUPERVISOR_SAFE_MODE_H

#include "py/mpconfig.h"

typedef enum {
    NO_SAFE_MODE = 0,
    BROWNOUT,
    HARD_CRASH,
    USER_SAFE_MODE,
    HEAP_OVERWRITTEN,
    MANUAL_SAFE_MODE,
    MICROPY_NLR_JUMP_FAIL,
    MICROPY_FATAL_ERROR,
    GC_ALLOC_OUTSIDE_VM,
    PROGRAMMATIC_SAFE_MODE,
    NORDIC_SOFT_DEVICE_ASSERT,
    FLASH_WRITE_FAIL,
    MEM_MANAGE,
    WATCHDOG_RESET,
    USB_TOO_MANY_ENDPOINTS,
    USB_TOO_MANY_INTERFACE_NAMES,
    NO_HEAP,
} safe_mode_t;

safe_mode_t wait_for_safe_mode_reset(void);

void safe_mode_on_next_reset(safe_mode_t reason);
void reset_into_safe_mode(safe_mode_t reason) NORETURN;

void print_safe_mode_message(safe_mode_t reason);

#endif  // MICROPY_INCLUDED_SUPERVISOR_SAFE_MODE_H
