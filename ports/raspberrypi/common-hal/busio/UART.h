// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/ringbuf.h"

#include "src/rp2_common/hardware_uart/include/hardware/uart.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint8_t cts_pin;
    uint8_t rs485_dir_pin;
    bool rs485_invert;
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
