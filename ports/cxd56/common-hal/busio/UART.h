// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    int8_t number;
    const mcu_pin_obj_t *tx_pin;
    const mcu_pin_obj_t *rx_pin;
    uint32_t baudrate;
    uint32_t timeout_us;
} busio_uart_obj_t;

void busio_uart_reset(void);
