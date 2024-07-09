// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "hal/include/hal_usart_async.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    struct usart_async_descriptor usart_desc;
    uint8_t rx_pin;
    uint8_t tx_pin;
    int8_t rts_pin;
    int8_t cts_pin;
    uint8_t character_bits;
    bool rx_error;
    uint32_t baudrate;
    uint32_t timeout_ms;
    uint32_t buffer_length;
    uint8_t *buffer;
} busio_uart_obj_t;
