/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_PSOC_EDGE_MACHINE_PIN_AF_H
#define MICROPY_INCLUDED_PSOC_EDGE_MACHINE_PIN_AF_H

#include "py/obj.h"
#include "gpio_pse84_bga_220.h"

typedef enum {
    MACHINE_PIN_AF_FN_I2C,
    MACHINE_PIN_AF_FN_SPI,
    MACHINE_PIN_AF_FN_UART,

    MACHINE_PIN_AF_FN_PDM,
    /* TODO: Add additional functionalities */
} machine_pin_af_fn_t;

typedef enum {
    MACHINE_PIN_AF_SIGNAL_I2C_SDA,
    MACHINE_PIN_AF_SIGNAL_I2C_SCL,

    MACHINE_PIN_AF_SIGNAL_SPI_MOSI,
    MACHINE_PIN_AF_SIGNAL_SPI_MISO,
    MACHINE_PIN_AF_SIGNAL_SPI_CLK,
    MACHINE_PIN_AF_SIGNAL_SPI_SELECT0,
    MACHINE_PIN_AF_SIGNAL_SPI_SELECT1,

    MACHINE_PIN_AF_SIGNAL_UART_TX,
    MACHINE_PIN_AF_SIGNAL_UART_RX,
    MACHINE_PIN_AF_SIGNAL_UART_CTS,
    MACHINE_PIN_AF_SIGNAL_UART_RTS,

    MACHINE_PIN_AF_SIGNAL_PDM_CLK,
    MACHINE_PIN_AF_SIGNAL_PDM_DATA,

    /* TODO: Add additional types */
} machine_pin_af_signal_t;

typedef struct {
    en_hsiom_sel_t idx;
    machine_pin_af_fn_t fn;
    uint8_t unit;
    machine_pin_af_signal_t signal;
    void *periph;
} machine_pin_af_obj_t;

extern const mp_obj_type_t machine_pin_af_type;

#endif // MICROPY_INCLUDED_PSOC_EDGE_MACHINE_PIN_AF_H
