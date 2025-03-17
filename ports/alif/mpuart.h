/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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
#ifndef MICROPY_INCLUDED_ALIF2_UART_H
#define MICROPY_INCLUDED_ALIF2_UART_H

#include "py/ringbuf.h"
#include "uart.h"

#define UART_MAX (8)
#define MP_UART_IRQ_RX (1)
#define MP_UART_IRQ_RXIDLE (2)
#define MP_UART_IRQ_TXIDLE (4)

void mp_uart_init(unsigned int uart_id, uint32_t baudrate,
    UART_DATA_BITS data_bits, UART_PARITY parity, UART_STOP_BITS stop_bits,
    mp_hal_pin_obj_t tx, mp_hal_pin_obj_t rx, ringbuf_t *rx_ringbuf);
void mp_uart_deinit(unsigned int uart_id);

void mp_uart_set_irq_callback(unsigned int uart_id, unsigned int trigger, void (*callback)(unsigned int uart_id, unsigned int trigger));
void mp_uart_set_flow(unsigned int uart_id, mp_hal_pin_obj_t rts, mp_hal_pin_obj_t cts);
void mp_uart_set_baudrate(unsigned int uart_id, uint32_t baudrate);
void mp_uart_set_bits_parity_stop(unsigned int uart_id, UART_DATA_BITS data_bits, UART_PARITY parity, UART_STOP_BITS stop_bits);

size_t mp_uart_rx_any(unsigned int uart_id);
size_t mp_uart_tx_any(unsigned int uart_id);
int mp_uart_rx_char(unsigned int uart_id);
void mp_uart_tx_data(unsigned int uart_id, const uint8_t *src, size_t len);

void mp_uart_init_repl(void);
void mp_uart_write_strn_repl(const char *str, size_t len);

#endif // MICROPY_INCLUDED_ALIF2_UART_H
