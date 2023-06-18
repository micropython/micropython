/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 - 2018 Glenn Ruben Bakke
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

#ifndef UART_H__
#define UART_H__

#include "pin.h"
#include "genhdr/pins.h"

typedef struct _machine_hard_uart_obj_t machine_hard_uart_obj_t;
extern const mp_obj_type_t machine_uart_type;

void uart_init0(void);
void uart_deinit(void);
void uart_irq_handler(mp_uint_t uart_id);

bool uart_rx_any(machine_hard_uart_obj_t *uart_obj);
int uart_rx_char(machine_hard_uart_obj_t *uart_obj);
void uart_tx_strn(machine_hard_uart_obj_t *uart_obj, const char *str, uint len);
void uart_tx_strn_cooked(machine_hard_uart_obj_t *uart_obj, const char *str, uint len);

#endif
