/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025
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

#include "semihosting_aarch64.h"

// AArch64 semihosting operation numbers
#define SYS_OPEN   0x01
#define SYS_CLOSE  0x02
#define SYS_WRITEC 0x03
#define SYS_WRITE0 0x04
#define SYS_WRITE  0x05
#define SYS_READ   0x06
#define SYS_READC  0x07
#define SYS_ISERROR 0x08
#define SYS_ISTTY  0x09
#define SYS_SEEK   0x0A
#define SYS_FLEN   0x0C
#define SYS_REMOVE 0x0E
#define SYS_RENAME 0x0F
#define SYS_CLOCK  0x10
#define SYS_TIME   0x11
#define SYS_SYSTEM 0x12
#define SYS_ERRNO  0x13
#define SYS_EXIT   0x18
#define SYS_GET_CMDLINE 0x15
#define SYS_HEAPINFO 0x16
#define SYS_EXIT_EXTENDED 0x20

// File open modes
#define OPEN_MODE_R   0  // "r"
#define OPEN_MODE_RB  1  // "rb"
#define OPEN_MODE_RP  2  // "r+"
#define OPEN_MODE_RPB 3  // "r+b"
#define OPEN_MODE_W   4  // "w"
#define OPEN_MODE_WB  5  // "wb"
#define OPEN_MODE_WP  6  // "w+"
#define OPEN_MODE_WPB 7  // "w+b"
#define OPEN_MODE_A   8  // "a"
#define OPEN_MODE_AB  9  // "ab"
#define OPEN_MODE_AP  10 // "a+"
#define OPEN_MODE_APB 11 // "a+b"

static int mp_semihosting_stdout_fd = -1;

static uint64_t mp_semihosting_call(uint64_t operation, const void *arg) {
    // AArch64 semihosting uses the HLT instruction with immediate 0xF000
    // x0 = operation number (input) / return value (output)
    // x1 = pointer to parameter block (input)
    register uint64_t x0 __asm__("x0") = operation;
    register const void *x1 __asm__("x1") = arg;
    __asm__ __volatile__ (
        "hlt 0xf000"
        : "+r" (x0)
        : "r" (x1)
        : "memory"
    );
    return x0;
}

static int mp_semihosting_open(const char *filename, size_t filename_len, uint64_t mode) {
    uint64_t args[3] = {
        (uint64_t)filename,
        mode,
        filename_len
    };
    return (int)mp_semihosting_call(SYS_OPEN, args);
}

static inline int mp_semihosting_close(int fd) {
    uint64_t args[1] = { (uint64_t)fd };
    return (int)mp_semihosting_call(SYS_CLOSE, args);
}

void mp_semihosting_init(void) {
    mp_semihosting_stdout_fd = mp_semihosting_open(":tt", 3, OPEN_MODE_W);
}

void mp_semihosting_exit(int status) {
    // ADP_Stopped_ApplicationExit = 0x20026
    uint64_t args[2] = { 0x20026, (uint64_t)status };
    mp_semihosting_call(SYS_EXIT_EXTENDED, args);
    // Should not return, but loop forever if it does
    while (1) {
        __asm__ volatile ("wfi");
    }
}

int mp_semihosting_rx_char(void) {
    return (int)mp_semihosting_call(SYS_READC, NULL);
}

int mp_semihosting_rx_chars(char *str, size_t len) {
    if (len == 0) {
        return 0;
    }
    struct {
        uint64_t fd;
        const char *str;
        uint64_t len;
    } args = {
        .fd = (uint64_t)mp_semihosting_stdout_fd,
        .str = str,
        .len = len
    };
    return (int)mp_semihosting_call(SYS_READ, &args);
}

static void mp_semihosting_tx_char(char c) {
    mp_semihosting_call(SYS_WRITEC, &c);
}

uint64_t mp_semihosting_tx_strn(const char *str, size_t len) {
    if (len == 0) {
        return 0;
    }
    if (len == 1) {
        mp_semihosting_tx_char(*str);
        return 0;
    }
    uint64_t args[3] = {
        (uint64_t)mp_semihosting_stdout_fd,
        (uint64_t)str,
        len
    };
    // SYS_WRITE returns number of bytes NOT written (0 = success)
    return mp_semihosting_call(SYS_WRITE, args);
}

uint64_t mp_semihosting_tx_strn_cooked(const char *str, size_t len) {
    // Write chunks of data until (excluding) the first '\n' character,
    // insert a '\r' character, and then continue with the next chunk.
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
