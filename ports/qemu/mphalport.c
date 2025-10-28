/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Damien P. George
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

#include "py/mphal.h"
#include "py/stream.h"
#include "shared/runtime/semihosting_arm.h"
#include "uart.h"

// UART is better behaved with redirection under qemu-system-arm, so prefer that for stdio.
#define USE_UART (1)
#define USE_SEMIHOSTING (0)

uintptr_t ticks_ms(void);
uintptr_t ticks_us(void);

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    #if USE_UART
    if ((poll_flags & MP_STREAM_POLL_RD) && uart_rx_any()) {
        return MP_STREAM_POLL_RD;
    }
    #endif
    return 0;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        #if USE_UART
        int c = uart_rx_chr();
        if (c >= 0) {
            return c;
        }
        #endif
        #if USE_SEMIHOSTING
        char str[1];
        int ret = mp_semihosting_rx_chars(str, 1);
        if (ret == 0) {
            return str[0];
        }
        #endif
    }
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    #if USE_UART
    uart_tx_strn(str, len);
    #endif
    #if USE_SEMIHOSTING
    mp_semihosting_tx_strn(str, len);
    #endif
    return len;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return ticks_ms();
}

mp_uint_t mp_hal_ticks_us(void) {
    return ticks_us();
}

void mp_hal_delay_ms(mp_uint_t ms) {
    mp_uint_t start = mp_hal_ticks_ms();
    while (mp_hal_ticks_ms() - start < ms) {
    }
}

void mp_hal_delay_us(mp_uint_t us) {
    mp_uint_t start = mp_hal_ticks_us();
    while (mp_hal_ticks_us() - start < us) {
    }
}

mp_uint_t mp_hal_ticks_cpu(void) {
    return 0;
}
