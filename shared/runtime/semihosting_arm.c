/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
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

#include "semihosting_arm.h"

// Resources:
// http://embed.rs/articles/2016/semi-hosting-rust/
// https://wiki.dlang.org/Minimal_semihosted_ARM_Cortex-M_%22Hello_World%22
// https://github.com/arduino/OpenOCD/blob/master/src/target/arm_semihosting.c

#define SYS_OPEN   0x01
#define SYS_WRITEC 0x03
#define SYS_WRITE  0x05
#define SYS_READC  0x07

// Constants:
#define OPEN_MODE_READ  (0) // mode "r"
#define OPEN_MODE_WRITE (4) // mode "w"

#ifndef __thumb__
#error Semihosting is only implemented for ARM microcontrollers.
#endif

static int mp_semihosting_stdout;

static uint32_t mp_semihosting_call(uint32_t num, const void *arg) {
    // A semihosting call works as follows, similar to a SVCall:
    //   * the call is invoked by a special breakpoint: 0xAB
    //   * the command is placed in r0
    //   * a pointer to the arguments is placed in r1
    //   * the return value is placed in r0
    // Note that because it uses the breakpoint instruction, applications
    // will hang if they're not connected to a debugger. And they'll be
    // stuck in a breakpoint if semihosting is not specifically enabled in
    // the debugger.
    // Also note that semihosting is extremely slow (sometimes >100ms per
    // call).
    register uint32_t num_reg __asm__ ("r0") = num;
    register const void *args_reg __asm__ ("r1") = arg;
    __asm__ __volatile__ (
        "bkpt 0xAB\n"         // invoke semihosting call
        : "+r" (num_reg)      // call number and result
        : "r" (args_reg)      // arguments
        : "memory");          // make sure args aren't optimized away
    return num_reg; // r0, which became the result
}

static int mp_semihosting_open_console(uint32_t mode) {
    struct {
        char *name;
        uint32_t mode;
        uint32_t name_len;
    } args = {
        .name = ":tt",     // magic path to console
        .mode = mode,      // e.g. "r", "w" (see OPEN_MODE_* constants)
        .name_len = 3,     // strlen(":tt")
    };
    return mp_semihosting_call(SYS_OPEN, &args);
}

void mp_semihosting_init() {
    mp_semihosting_stdout = mp_semihosting_open_console(OPEN_MODE_WRITE);
}

int mp_semihosting_rx_char() {
    return mp_semihosting_call(SYS_READC, NULL);
}

static void mp_semihosting_tx_char(char c) {
    mp_semihosting_call(SYS_WRITEC, &c);
}

uint32_t mp_semihosting_tx_strn(const char *str, size_t len) {
    if (len == 0) {
        return 0; // nothing to do
    }
    if (len == 1) {
        mp_semihosting_tx_char(*str); // maybe faster?
        return 0;
    }

    struct {
        uint32_t fd;
        const char *str;
        uint32_t len;
    } args = {
        .fd = mp_semihosting_stdout,
        .str = str,
        .len = len,
    };
    return mp_semihosting_call(SYS_WRITE, &args);
}

uint32_t mp_semihosting_tx_strn_cooked(const char *str, size_t len) {
    // Write chunks of data until (excluding) the first '\n' character,
    // insert a '\r' character, and then continue with the next chunk
    // (starting with '\n').
    // Doing byte-by-byte writes would be easier to implement but is far
    // too slow.
    size_t start = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '\n') {
            mp_semihosting_tx_strn(str + start, i - start);
            mp_semihosting_tx_char('\r');
            start = i;
        }
    }
    return mp_semihosting_tx_strn(str + start, len - start);
}
