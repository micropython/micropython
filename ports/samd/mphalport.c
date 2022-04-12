/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#include "py/mphal.h"
#include "py/stream.h"
#include "shared/runtime/interrupt_char.h"
#include "samd_soc.h"
#include "tusb.h"

#ifndef MICROPY_HW_STDIN_BUFFER_LEN
#define MICROPY_HW_STDIN_BUFFER_LEN 256
#endif

STATIC uint8_t stdin_ringbuf_array[MICROPY_HW_STDIN_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0 };

uint8_t cdc_itf_pending; // keep track of cdc interfaces which need attention to poll

void poll_cdc_interfaces(void) {
    // any CDC interfaces left to poll?
    if (cdc_itf_pending && ringbuf_free(&stdin_ringbuf)) {
        for (uint8_t itf = 0; itf < 8; ++itf) {
            if (cdc_itf_pending & (1 << itf)) {
                tud_cdc_rx_cb(itf);
                if (!cdc_itf_pending) {
                    break;
                }
            }
        }
    }
}

void tud_cdc_rx_cb(uint8_t itf) {
    // consume pending USB data immediately to free usb buffer and keep the endpoint from stalling.
    // in case the ringbuffer is full, mark the CDC interface that need attention later on for polling
    cdc_itf_pending &= ~(1 << itf);
    for (uint32_t bytes_avail = tud_cdc_n_available(itf); bytes_avail > 0; --bytes_avail) {
        if (ringbuf_free(&stdin_ringbuf)) {
            int data_char = tud_cdc_read_char();
            #if MICROPY_KBD_EXCEPTION
            if (data_char == mp_interrupt_char) {
                mp_sched_keyboard_interrupt();
            } else {
                ringbuf_put(&stdin_ringbuf, data_char);
            }
            #else
            ringbuf_put(&stdin_ringbuf, data_char);
            #endif
        } else {
            cdc_itf_pending |= (1 << itf);
            return;
        }
    }
}

void mp_hal_delay_ms(mp_uint_t ms) {
    ms += 1;
    uint32_t t0 = systick_ms;
    while (systick_ms - t0 < ms) {
        MICROPY_EVENT_POLL_HOOK
    }
}

void mp_hal_delay_us(mp_uint_t us) {
    uint32_t ms = us / 1000 + 1;
    uint32_t t0 = systick_ms;
    while (systick_ms - t0 < ms) {
        __WFI();
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if (enable_uart_repl && USARTx->USART.INTFLAG.bit.RXC) {
        ret |= MP_STREAM_POLL_RD;
    }

    poll_cdc_interfaces();
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_peek(&stdin_ringbuf) != -1) {
        ret |= MP_STREAM_POLL_RD;
    }

    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        if (enable_uart_repl && USARTx->USART.INTFLAG.bit.RXC) {
            return USARTx->USART.DATA.bit.DATA;
        }

        poll_cdc_interfaces();
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        __WFI();
    }
}

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    if (tud_cdc_connected()) {
        for (size_t i = 0; i < len;) {
            uint32_t n = len - i;
            if (n > CFG_TUD_CDC_EP_BUFSIZE) {
                n = CFG_TUD_CDC_EP_BUFSIZE;
            }
            while (n > tud_cdc_write_available()) {
                __WFI();
            }
            uint32_t n2 = tud_cdc_write(str + i, n);
            tud_cdc_write_flush();
            i += n2;
        }
    }
    if (enable_uart_repl) {
        while (len--) {
            while (!(USARTx->USART.INTFLAG.bit.DRE)) {
            }
            USARTx->USART.DATA.bit.DATA = *str++;
        }
    }
}
