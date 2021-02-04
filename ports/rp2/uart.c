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
#include "py/ringbuf.h"
#include "py/mphal.h"
#include "uart.h"

#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/regs/uart.h"

#if MICROPY_HW_ENABLE_UART_REPL

void uart_irq(void) {
    uart_get_hw(uart_default)->icr = UART_UARTICR_BITS; // clear interrupt flags
    if (uart_is_readable(uart_default)) {
        int c = uart_getc(uart_default);
        #if MICROPY_KBD_EXCEPTION
        if (c == mp_interrupt_char) {
            mp_keyboard_interrupt();
            return;
        }
        #endif
        ringbuf_put(&stdin_ringbuf, c);
    }
}

void mp_uart_init(void) {
    uart_get_hw(uart_default)->imsc = UART_UARTIMSC_BITS; // enable mask
    uint irq_num = uart_get_index(uart_default) ? UART1_IRQ : UART0_IRQ;
    irq_set_exclusive_handler(irq_num, uart_irq);
    irq_set_enabled(irq_num, true); // enable irq
}

void mp_uart_write_strn(const char *str, size_t len) {
    uart_write_blocking(uart_default, (const uint8_t *)str, len);
}

#endif
