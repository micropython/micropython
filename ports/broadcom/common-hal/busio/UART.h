// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/ringbuf.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *tx_pin;
    const mcu_pin_obj_t *rx_pin;
    const mcu_pin_obj_t *cts_pin;
    const mcu_pin_obj_t *rts_pin;
    uint32_t baudrate;
    uint32_t timeout_ms;
    ringbuf_t ringbuf;
    bool sigint_enabled;
    uint8_t uart_id;
} busio_uart_obj_t;

extern void reset_uart(void);
