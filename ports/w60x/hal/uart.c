/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "wm_include.h"
#include "mphalport.h"

#include "py/mpstate.h"
#include "py/mphal.h"

extern int sendchar(int ch);

STATIC uint8_t stdin_ringbuf_array[256];
ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array)};
STATIC tls_os_sem_t *w600_uart0_rx_sem = NULL;

// Receive single character
int mp_hal_stdin_rx_chr(void)
#if 1
{
    for (;;) {
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        //MICROPY_EVENT_POLL_HOOK
        //ulTaskNotifyTake(pdFALSE, 1);
        //tls_os_time_delay(0);
        tls_os_sem_acquire(w600_uart0_rx_sem, 0);
    }
}
#endif
#if 0
{
    unsigned char c = 0;

    // wait for RXNE
    while(0 == (tls_reg_read32(HR_UART0_FIFO_STATUS)&UFS_RX_FIFO_CNT_MASK))
        tls_os_time_delay(1);
    c = tls_reg_read32(HR_UART0_RX_WIN);

    return c;
}
#endif

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, size_t len) {
while (len--) {
        // wait for TXE
        sendchar(*str++);
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

// Efficiently convert "\n" to "\r\n"
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    const char *last = str;
    while (len--) {
        if (*str == '\n') {
            if (str > last) {
                mp_hal_stdout_tx_strn(last, str - last);
            }
            mp_hal_stdout_tx_strn("\r\n", 2);
            ++str;
            last = str;
        } else {
            ++str;
        }
    }
    if (str > last) {
        mp_hal_stdout_tx_strn(last, str - last);
    }
}

STATIC s16 uart_rx_callback(u16 len)
{
    uint8_t c;
    while(tls_uart_read(TLS_UART_0, &c, 1) > 0)
    {
        if (c == mp_interrupt_char) {
            // inline version of mp_keyboard_interrupt();
            MP_STATE_VM(mp_pending_exception) = MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception));
            #if MICROPY_ENABLE_SCHEDULER
            if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
                MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
            }
            #endif
        } else {
            // this is an inline function so will be in IRAM
            ringbuf_put(&stdin_ringbuf, c);
        }
    }

    tls_os_sem_release(w600_uart0_rx_sem);
}

void uart_init(void)
{
    tls_os_sem_create(&w600_uart0_rx_sem, 0);
    tls_uart_port_init(TLS_UART_0, NULL, 0);
    tls_uart_rx_callback_register((u16) TLS_UART_0, uart_rx_callback);
}

