// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_MAX32_COMMON_HAL_BUSIO_UART_H
#define MICROPY_INCLUDED_MAX32_COMMON_HAL_BUSIO_UART_H

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"
#include "py/ringbuf.h"

#include "max32_port.h"

// Define a struct for what BUSIO.UART should contain
typedef struct {
    mp_obj_base_t base;
    int error;
    float timeout;

    int uart_id;
    int uart_map;
    mxc_uart_regs_t *uart_regs;
    ringbuf_t ringbuf;
    bool parity;
    uint8_t bits;
    uint8_t stop_bits;
    uint32_t baudrate;

    const mcu_pin_obj_t *rx_pin;
    const mcu_pin_obj_t *tx_pin;
    const mcu_pin_obj_t *rts_pin;
    const mcu_pin_obj_t *cts_pin;
} busio_uart_obj_t;

void uart_reset(void);

#endif  // MICROPY_INCLUDED_MAX32_COMMON_HAL_BUSIO_UART_H
