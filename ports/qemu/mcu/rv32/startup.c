/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Alessandro Gatti
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "uart.h"
#include "py/mpconfig.h"
#include "shared/runtime/semihosting.h"

extern void set_interrupt_table(void);
extern int main(int argc, char **argv);

void _entry_point(void) {
    // Set interrupt table
    set_interrupt_table();
    // Initialise semihosting
    mp_semihosting_init();
    // Enable UART
    uart_init();
    // Now that we have a basic system up and running we can call main
    main(0, 0);
    // Finished
    exit(0);
}

void exit(int status) {
    mp_semihosting_terminate(MP_SEMIHOSTING_EXIT_APPLICATION_EXIT, status);
    MP_UNREACHABLE;
}

#ifndef NDEBUG
void __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)func;
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    exit(1);
}
#endif

// Picolibc requires `stdout` to be explicitly defined.

#ifdef _PICOLIBC__
FILE *const stdout;
#endif
