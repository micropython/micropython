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
    SAFE_MODE_NONE = 0,
    // BROWNOUT should be lowest after SAFE_MODE_NONE.
    SAFE_MODE_BROWNOUT,
    // alphabetical from here down
    SAFE_MODE_FLASH_WRITE_FAIL,
    SAFE_MODE_GC_ALLOC_OUTSIDE_VM,
    SAFE_MODE_HARD_FAULT,
    SAFE_MODE_INTERRUPT_ERROR,
    SAFE_MODE_MANUAL,
    SAFE_MODE_NLR_JUMP_FAIL,
    SAFE_MODE_NO_CIRCUITPY,
    SAFE_MODE_NO_HEAP,
    SAFE_MODE_PROGRAMMATIC,
    SAFE_MODE_SAFEMODE_PY_ERROR,
    SAFE_MODE_SDK_FATAL_ERROR,
    SAFE_MODE_STACK_OVERFLOW,
    SAFE_MODE_USB_BOOT_DEVICE_NOT_INTERFACE_ZERO,
    SAFE_MODE_USB_TOO_MANY_ENDPOINTS,
    SAFE_MODE_USB_TOO_MANY_INTERFACE_NAMES,
    SAFE_MODE_USER,
    SAFE_MODE_WATCHDOG,
} safe_mode_t;

safe_mode_t get_safe_mode(void);
void set_safe_mode(safe_mode_t safe_mode);

safe_mode_t wait_for_safe_mode_reset(void);

void safe_mode_on_next_reset(safe_mode_t reason);
void reset_into_safe_mode(safe_mode_t reason) NORETURN;

void print_safe_mode_message(safe_mode_t reason);

#endif  // MICROPY_INCLUDED_SUPERVISOR_SAFE_MODE_H
