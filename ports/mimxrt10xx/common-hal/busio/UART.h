// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/ringbuf.h"
#include "py/obj.h"
#include "periph.h"

#include "sdk/drivers/lpuart/fsl_lpuart.h"

typedef struct {
    mp_obj_base_t base;
    LPUART_Type *uart;
    lpuart_handle_t handle;
    uint8_t *ringbuf;
    uint32_t baudrate;
    uint32_t timeout_ms;
    uint8_t character_bits;
    uint8_t index;
    const mcu_pin_obj_t *rx;
    const mcu_pin_obj_t *tx;
    const mcu_pin_obj_t *cts;
    const mcu_pin_obj_t *rts;
    const mcu_pin_obj_t *rs485_dir;
    bool rs485_invert;

} busio_uart_obj_t;
