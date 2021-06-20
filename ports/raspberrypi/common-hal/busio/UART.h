/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#ifndef MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_BUSIO_UART_H
#define MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_BUSIO_UART_H

#include "py/obj.h"
#include "py/ringbuf.h"

#include "src/rp2_common/hardware_uart/include/hardware/uart.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint8_t cts_pin;
    uint8_t rts_pin;
    uint8_t uart_id;
    uint8_t uart_irq_id;
    uint32_t baudrate;
    uint32_t timeout_ms;
    uart_inst_t *uart;
    ringbuf_t ringbuf;
} busio_uart_obj_t;

extern void reset_uart(void);
extern void never_reset_uart(uint8_t num);

#endif // MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_BUSIO_UART_H
