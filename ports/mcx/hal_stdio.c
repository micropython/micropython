/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/ringbuf.h"
#include "extmod/misc.h"
#include "tusb.h"

#include "drv_uart.h"
#include "pendsv.h"

static uint8_t s_stdin_rb_array[512];
static ringbuf_t s_stdin_rb = {
    .buf = s_stdin_rb_array,
    .size = sizeof(s_stdin_rb_array),
};

static void mp_hal_stdin_intr_callback(void *handle, void *param, uint8_t len);

int mp_hal_stdio_init(void) {
    drv_uart_t *uart = MP_STATE_PORT(stdio_uart);

    return drv_uart_async_start(uart, mp_hal_stdin_intr_callback, &s_stdin_rb);
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    return mp_os_dupterm_poll(poll_flags);
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    mp_uint_t ret = len;
    bool write_done = false;

    #if MICROPY_HW_ENABLE_UART_REPL
    drv_uart_t *uart = MP_STATE_PORT(stdio_uart);
    drv_uart_write(uart, (const uint8_t *)str, len, 1000);

    write_done = true;
    #endif

    #if MICROPY_HW_USB_CDC
    if (tud_cdc_connected()) {
        size_t i = 0;
        while (i < len) {
            uint32_t n = len - i;
            if (n > CFG_TUD_CDC_EP_BUFSIZE) {
                n = CFG_TUD_CDC_EP_BUFSIZE;
            }
            int timeout = 0;
            // Wait with a max of USC_CDC_TIMEOUT ms
            while (n > tud_cdc_write_available() && timeout++ < 1000) {
                MICROPY_EVENT_POLL_HOOK
            }
            if (timeout >= 1000) {
                break;
            }
            uint32_t n2 = tud_cdc_write(str + i, n);
            tud_cdc_write_flush();
            i += n2;
        }

        write_done = true;
    }
    #endif

    #if MICROPY_PY_OS_DUPTERM
    int dupterm_res = mp_os_dupterm_tx_strn(str, len);
    if (dupterm_res >= 0) {
        ret = MIN((mp_uint_t)dupterm_res, ret);
    }
    #endif

    if (!write_done) {
        ret = 0U;
    }

    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    char ch = 0x00;
    for (;;) {

        if (ringbuf_avail(&s_stdin_rb)) {
            ch = ringbuf_get(&s_stdin_rb);
            break;
        }
        MICROPY_EVENT_POLL_HOOK
    }

    return ch;
}

void tud_cdc_rx_cb(uint8_t itf) {
    /* TODO: This implementation is not safe when UART ISR is interrupted by USB ISR. */
    /* Both terminals will write to the same ring-buffer, which is not guarded. */

    uint32_t bytes_avail = tud_cdc_n_available(itf);

    while (bytes_avail > 0) {
        int ch = tud_cdc_read_char();
        if (ch == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
        } else {
            ringbuf_put(&s_stdin_rb, ch);
        }

        bytes_avail--;
    }
}

static void mp_hal_stdin_intr_callback(void *handle, void *param, uint8_t len) {
    ringbuf_t *rb = (ringbuf_t *)param;
    drv_uart_t *uart = (drv_uart_t *)handle;

    uint8_t ch;

    for (uint8_t i = 0; i < len; i++) {
        drv_uart_async_read(uart, &ch, 1);
        if (ch == mp_interrupt_char) {
            pendsv_kbd_intr();
        } else {
            ringbuf_put(rb, ch);
        }
    }
}

MP_REGISTER_ROOT_POINTER(struct drv_uart_type *stdio_uart);
