// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "nrfx_uarte.h"

#include "py/obj.h"
#include "py/ringbuf.h"

typedef struct {
    mp_obj_base_t base;

    nrfx_uarte_t *uarte;

    uint32_t baudrate;
    uint32_t timeout_ms;

    ringbuf_t ringbuf;
    uint8_t rx_char;    // EasyDMA buf
    bool rx_paused;     // set by irq if no space in rbuf

    uint8_t tx_pin_number;
    uint8_t rx_pin_number;
    uint8_t cts_pin_number;
    uint8_t rts_pin_number;
} busio_uart_obj_t;

void uart_reset(void);
