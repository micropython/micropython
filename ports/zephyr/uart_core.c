/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Linaro Limited
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
#include <unistd.h>
#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "src/zephyr_getchar.h"
// Zephyr headers
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/console/console.h>
#include <zephyr/drivers/uart.h>

/*
 * Core UART functions to implement for a port
 */

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if (poll_flags & MP_STREAM_POLL_RD) {
        #ifdef CONFIG_CONSOLE_SUBSYS
        // It's not easy to test if tty is readable, so just unconditionally set it for now.
        ret |= MP_STREAM_POLL_RD;
        #else
        if (zephyr_getchar_check()) {
            ret |= MP_STREAM_POLL_RD;
        }
        #endif
    }
    if (poll_flags & MP_STREAM_POLL_WR) {
        ret |= MP_STREAM_POLL_WR;
    }
    return ret;
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        int _chr;
        #ifdef CONFIG_CONSOLE_SUBSYS
        _chr = console_getchar();
        #else
        _chr = zephyr_getchar();
        #endif
        if (_chr >= 0) {
            return _chr;
        }
        mp_event_wait_ms(1);
    }
}

// Send string of given length
mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t ret = len;
    #ifdef CONFIG_CONSOLE_SUBSYS
    while (len--) {
        char c = *str++;
        /* console_putchar returns -EAGAIN when no free tx is available */
        while (console_putchar(c) == -EAGAIN) {
            mp_event_wait_ms(1);
        }
    }
    #else
    static const struct device *uart_console_dev =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

    while (len--) {
        uart_poll_out(uart_console_dev, *str++);
    }
    #endif
    return ret;
}
