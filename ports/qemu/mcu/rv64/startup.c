/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Alessandro Gatti
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

extern void set_interrupt_table(void);
extern int main(int argc, char **argv);

void _entry_point(void) {
    // Set interrupt table
    set_interrupt_table();
    // Enable UART
    uart_init();
    // Now that we have a basic system up and running we can call main
    main(0, 0);
    // Finished
    exit(0);
}

void exit(int status) {
    uintptr_t semihosting_arguments[2] = { 0 };

    // Exit via QEMU's RISC-V semihosting support.
    __asm volatile (
        ".option push            \n" // Transient options
        ".option norvc           \n" // Do not emit compressed instructions
        ".align 4                \n" // 16 bytes alignment
        "mv     a1, %0           \n" // Load buffer
        "li     t0, 0x20026      \n" // ADP_Stopped_ApplicationExit
        "sd     t0, 0(a1)        \n" // ADP_Stopped_ApplicationExit
        "sd     %1, 8(a1)        \n" // Exit code
        "addi   a0, zero, 0x20   \n" // TARGET_SYS_EXIT_EXTENDED
        "slli   zero, zero, 0x1F \n" // Entry NOP
        "ebreak                  \n" // Give control to the debugger
        "srai   zero, zero, 7    \n" // Semihosting call
        ".option pop             \n" // Restore previous options set
        :
        : "r" (semihosting_arguments), "r" (status)
        : "memory"
        );

    // Should never reach here.
    for (;;) {
    }
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
