/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "py/mphal.h"
#include "shared/timeutils/timeutils.h"
#include "tusb.h"
#include "uart.h"
#include "hardware/rtc.h"

#if MICROPY_HW_ENABLE_UART_REPL

#ifndef UART_BUFFER_LEN
// reasonably big so we can paste
#define UART_BUFFER_LEN 256
#endif

STATIC uint8_t stdin_ringbuf_array[UART_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array) };

#endif

#if MICROPY_KBD_EXCEPTION

int mp_interrupt_char = -1;

void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {
    (void)itf;
    (void)wanted_char;
    tud_cdc_read_char(); // discard interrupt char
    mp_sched_keyboard_interrupt();
}

void mp_hal_set_interrupt_char(int c) {
    mp_interrupt_char = c;
    tud_cdc_set_wanted_char(c);
}

#endif

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    #if MICROPY_HW_ENABLE_UART_REPL
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_peek(&stdin_ringbuf) != -1) {
        ret |= MP_STREAM_POLL_RD;
    }
    #endif
    #if MICROPY_HW_ENABLE_USBDEV
    if (tud_cdc_connected() && tud_cdc_available()) {
        ret |= MP_STREAM_POLL_RD;
    }
    #endif
    return ret;
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        #if MICROPY_HW_ENABLE_UART_REPL
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        #endif
        #if MICROPY_HW_ENABLE_USBDEV
        if (tud_cdc_connected() && tud_cdc_available()) {
            uint8_t buf[1];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            if (count) {
                return buf[0];
            }
        }
        #endif
        MICROPY_EVENT_POLL_HOOK
    }
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    #if MICROPY_HW_ENABLE_UART_REPL
    mp_uart_write_strn(str, len);
    #endif

    #if MICROPY_HW_ENABLE_USBDEV
    if (tud_cdc_connected()) {
        for (size_t i = 0; i < len;) {
            uint32_t n = len - i;
            if (n > CFG_TUD_CDC_EP_BUFSIZE) {
                n = CFG_TUD_CDC_EP_BUFSIZE;
            }
            while (n > tud_cdc_write_available()) {
                tud_task();
                tud_cdc_write_flush();
            }
            uint32_t n2 = tud_cdc_write(str + i, n);
            tud_task();
            tud_cdc_write_flush();
            i += n2;
        }
    }
    #endif
}

void mp_hal_delay_ms(mp_uint_t ms) {
    absolute_time_t t = make_timeout_time_ms(ms);
    while (!time_reached(t)) {
        mp_handle_pending(true);
        best_effort_wfe_or_timeout(t);
        MICROPY_HW_USBDEV_TASK_HOOK
    }
}

uint64_t mp_hal_time_ns(void) {
    datetime_t t;
    rtc_get_datetime(&t);
    uint64_t s = timeutils_seconds_since_epoch(t.year, t.month, t.day, t.hour, t.min, t.sec);
    return s * 1000000000ULL;
}
