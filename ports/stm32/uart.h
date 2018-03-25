/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_UART_H
#define MICROPY_INCLUDED_STM32_UART_H

typedef enum {
    PYB_UART_NONE = 0,
    PYB_UART_1 = 1,
    PYB_UART_2 = 2,
    PYB_UART_3 = 3,
    PYB_UART_4 = 4,
    PYB_UART_5 = 5,
    PYB_UART_6 = 6,
    PYB_UART_7 = 7,
    PYB_UART_8 = 8,
} pyb_uart_t;

typedef struct _pyb_uart_obj_t pyb_uart_obj_t;
extern const mp_obj_type_t pyb_uart_type;

void uart_init0(void);
void uart_deinit(void);
void uart_irq_handler(mp_uint_t uart_id);

mp_uint_t uart_rx_any(pyb_uart_obj_t *uart_obj);
int uart_rx_char(pyb_uart_obj_t *uart_obj);
void uart_tx_strn(pyb_uart_obj_t *uart_obj, const char *str, uint len);

#endif // MICROPY_INCLUDED_STM32_UART_H
