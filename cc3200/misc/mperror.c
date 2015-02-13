/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include <std.h>
#include <stdint.h>
#include <string.h>

#include "mpconfig.h"
#include MICROPY_HAL_H
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "inc/hw_memmap.h"
#include "pybuart.h"
#include "utils.h"


void NORETURN __fatal_error(const char *msg) {
    if (msg != NULL) {
        // wait for 20ms
        UtilsDelay(UTILS_DELAY_US_TO_COUNT(20000));
        mp_hal_stdout_tx_str("\r\nFATAL ERROR:");
        mp_hal_stdout_tx_str(msg);
        mp_hal_stdout_tx_str("\r\n");
    }
    for ( ;; ) {__WFI();}
}

void __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void) func;
    printf("Assertion failed: %s, file %s, line %d\n", expr, file, line);
    __fatal_error(NULL);
}

void nlr_jump_fail(void *val) {
#ifdef DEBUG
    char msg[64];
    snprintf(msg, sizeof(msg), "uncaught exception %p\n", val);
    __fatal_error(msg);
#else
    __fatal_error(NULL);
#endif
}

