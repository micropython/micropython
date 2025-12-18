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
#include <zephyr/console/tty.h>
#include <zephyr/drivers/uart.h>


#ifdef CONFIG_CONSOLE_SUBSYS

static int mp_console_putchar(char c);
static int mp_console_getchar(void);


static struct tty_serial mp_console_serial;

static uint8_t mp_console_rxbuf[CONFIG_CONSOLE_GETCHAR_BUFSIZE];
static uint8_t mp_console_txbuf[CONFIG_CONSOLE_PUTCHAR_BUFSIZE];

#endif  // CONFIG_CONSOLE_SUBSYS

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
        _chr = mp_console_getchar();
        #else
        _chr = zephyr_getchar();
        #endif
        if (_chr >= 0) {
            return _chr;
        }
        MICROPY_EVENT_POLL_HOOK
    }
}

// Send string of given length
mp_uint_t mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    mp_uint_t ret = len;
    #ifdef CONFIG_CONSOLE_SUBSYS
    while (len--) {
        char c = *str++;
        while (mp_console_putchar(c) == -1) {
            MICROPY_EVENT_POLL_HOOK
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


#ifdef CONFIG_CONSOLE_SUBSYS

int mp_console_init(void) {

    const struct device *uart_dev;
    int ret;

    uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
    if (!device_is_ready(uart_dev)) {
        return -ENODEV;
    }

    ret = tty_init(&mp_console_serial, uart_dev);

    if (ret) {
        return ret;
    }

    /* Checks device driver supports for interrupt driven data transfers. */
    if (CONFIG_CONSOLE_GETCHAR_BUFSIZE + CONFIG_CONSOLE_PUTCHAR_BUFSIZE) {
        const struct uart_driver_api *api =
            (const struct uart_driver_api *)uart_dev->api;
        if (!api->irq_callback_set) {
            return -ENOTSUP;
        }
    }

    tty_set_tx_buf(&mp_console_serial, mp_console_txbuf, sizeof(mp_console_txbuf));
    tty_set_rx_buf(&mp_console_serial, mp_console_rxbuf, sizeof(mp_console_rxbuf));

    tty_set_rx_timeout(&mp_console_serial, 0);
    tty_set_tx_timeout(&mp_console_serial, 1);

    return 0;
}

static int mp_console_putchar(char c) {
    return tty_write(&mp_console_serial, &c, 1);
}

static int mp_console_getchar(void) {
    uint8_t c;
    int res;

    res = tty_read(&mp_console_serial, &c, 1);
    if (res < 0) {
        return res;
    }

    return c;
}

#endif  // CONFIG_CONSOLE_SUBSYS
