/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jim Mussared
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

#ifndef MICROPY_INCLUDED_EXTMOD_NIMBLE_HAL_HAL_UART_H
#define MICROPY_INCLUDED_EXTMOD_NIMBLE_HAL_HAL_UART_H

#include <stdint.h>

#define SYSINIT_PANIC_ASSERT_MSG(cond, msg)

#define HAL_UART_PARITY_NONE (0)

typedef int (*hal_uart_tx_cb_t)(void *arg);
typedef int (*hal_uart_rx_cb_t)(void *arg, uint8_t data);

// --- Called by NimBLE, implemented in hal_uart.c. ---------------------------
int hal_uart_init_cbs(uint32_t port, hal_uart_tx_cb_t tx_cb, void *tx_arg, hal_uart_rx_cb_t rx_cb, void *rx_arg);
int hal_uart_config(uint32_t port, uint32_t baud, uint32_t bits, uint32_t stop, uint32_t parity, uint32_t flow);
void hal_uart_start_tx(uint32_t port);
int hal_uart_close(uint32_t port);

// --- Called by the MicroPython port when UART data is available -------------
void mp_bluetooth_nimble_hci_uart_process(bool run_events);

#endif // MICROPY_INCLUDED_EXTMOD_NIMBLE_HAL_HAL_UART_H
