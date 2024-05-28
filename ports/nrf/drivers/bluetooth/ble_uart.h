/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#ifndef BLUETOOTH_LE_UART_H__
#define BLUETOOTH_LE_UART_H__

#if BLUETOOTH_SD

#include "modubluepy.h"
#include "ble_drv.h"

void ble_uart_init0(void);
void ble_uart_advertise(void);
bool ble_uart_connected(void);
bool ble_uart_enabled(void);

uintptr_t mp_ble_uart_stdio_poll(uintptr_t poll_flags);
int mp_ble_uart_stdin_rx_chr(void);
mp_uint_t mp_ble_uart_stdout_tx_strn(const char *str, size_t len);

#endif // BLUETOOTH_SD

#endif // BLUETOOTH_LE_UART_H__
