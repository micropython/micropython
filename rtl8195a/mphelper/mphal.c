/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Chester Tseng
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
#include "osdep_api.h"
#include "device.h"
#include "log_uart_api.h"

static log_uart_t log_uart_obj;

void mp_hal_log_uart_init(void)
{
    log_uart_init(&log_uart_obj, 38400, 8, ParityNone, 1);
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        int c = log_uart_getc(&log_uart_obj);
        if (c != -1) {
            return c;
        }
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    while (*str) {
        log_uart_putc(&log_uart_obj, *str++);
    }
}

void mp_hal_stdout_tx_strn(const char *str, uint32_t len) {
    while (len--) {
        log_uart_putc(&log_uart_obj, *str++);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, uint32_t len) {
    while (len--) {
        if (*str == '\n') {
            log_uart_putc(&log_uart_obj, '\r');
        }
        log_uart_putc(&log_uart_obj, *str++);
    }
}

void mp_hal_set_interrupt_char(int c) {
    mpexception_set_interrupt_char (c);
}

void mp_hal_delay_ms(uint32_t ms) {
    //TODO
    mdelay(ms);
}

void mp_hal_delay_us(uint32_t us) {
    //TODO
    udelay(us);
}

uint32_t mp_hal_ticks_ms(void) {
    // TODO
    return 0;
}


