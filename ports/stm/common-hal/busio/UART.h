/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_STM32_COMMON_HAL_BUSIO_UART_H
#define MICROPY_INCLUDED_STM32_COMMON_HAL_BUSIO_UART_H

#include "common-hal/microcontroller/Pin.h"
#include "peripherals/periph.h"

#include "py/obj.h"
#include "py/ringbuf.h"

#ifndef UART_IRQPRI
#define UART_IRQPRI       1
#endif
#ifndef UART_IRQSUB_PRI
#define UART_IRQSUB_PRI    0
#endif

typedef struct {
    mp_obj_base_t base;
    UART_HandleTypeDef handle;
    IRQn_Type irq;
    const mcu_periph_obj_t *tx;
    const mcu_periph_obj_t *rx;

    ringbuf_t ringbuf;
    uint8_t rx_char;

    uint32_t baudrate;
    uint32_t timeout_ms;

    bool sigint_enabled;
} busio_uart_obj_t;

void uart_reset(void);

#endif // MICROPY_INCLUDED_STM32F4_COMMON_HAL_BUSIO_UART_H
