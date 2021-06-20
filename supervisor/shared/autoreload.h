/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SUPERVISOR_AUTORELOAD_H
#define MICROPY_INCLUDED_SUPERVISOR_AUTORELOAD_H

#include <stdbool.h>

#include "supervisor/memory.h"

enum {
    SUPERVISOR_NEXT_CODE_OPT_RELOAD_ON_SUCCESS = 0x1,
    SUPERVISOR_NEXT_CODE_OPT_RELOAD_ON_ERROR = 0x2,
    SUPERVISOR_NEXT_CODE_OPT_STICKY_ON_SUCCESS = 0x4,
    SUPERVISOR_NEXT_CODE_OPT_STICKY_ON_ERROR = 0x8,
    SUPERVISOR_NEXT_CODE_OPT_STICKY_ON_RELOAD = 0x10,
    SUPERVISOR_NEXT_CODE_OPT_NEWLY_SET = 0x20,
};

typedef struct {
    uint8_t options;
    char filename[];
} next_code_info_t;

extern supervisor_allocation *next_code_allocation;

extern volatile bool reload_requested;

void autoreload_tick(void);

void autoreload_start(void);
void autoreload_stop(void);
void autoreload_enable(void);
void autoreload_disable(void);
bool autoreload_is_enabled(void);

// Temporarily turn it off. Used during the REPL.
void autoreload_suspend(void);
void autoreload_resume(void);

void autoreload_now(void);

#endif  // MICROPY_INCLUDED_SUPERVISOR_AUTORELOAD_H
