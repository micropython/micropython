/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Analog Devices, Inc.
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
#define asm __asm

#include <sys/adi_core.h>
#include <sys/platform.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/unistd.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/mp-readline/readline.h"
#include "lib/utils/pyexec.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include <services/rtc/adi_rtc.h>
#include "adi_initialize.h"
#include "bm_uart.h"
#include "pin.h"
#include "modmachine.h"
#include "systick.h"

BM_UART uart0;

int main(int argc, char **argv) {

    adi_initComponents();

    uart_initialize(&uart0, UART_BAUD_RATE_115200, UART_SERIAL_8N1, UART0);
    uart_write_byte(&uart0, 0x0C); // Clear the screen

    adi_rtc_Init();

    mp_init();
    machine_init();
    readline_init0();
    pin_init0();
    sys_tick_init();
    sdcard_init();

    pyexec_friendly_repl();

    machine_deinit();
    mp_deinit();

    while(1);

    return 0; // never reaches here
}

void nlr_jump_fail(void *val) {
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    while(1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    while(1);
}
#endif

