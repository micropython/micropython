// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

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
