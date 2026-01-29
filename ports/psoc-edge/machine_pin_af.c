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

#include "machine_pin_af.h"

const char *machine_pin_af_signal_str[] = {
    [MACHINE_PIN_AF_SIGNAL_I2C_SDA] = "I2C_SDA",
    [MACHINE_PIN_AF_SIGNAL_I2C_SCL] = "I2C_SCL",

    [MACHINE_PIN_AF_SIGNAL_SPI_MOSI] = "SPI_MOSI",
    [MACHINE_PIN_AF_SIGNAL_SPI_MISO] = "SPI_MISO",
    [MACHINE_PIN_AF_SIGNAL_SPI_CLK] = "SPI_CLK",
    [MACHINE_PIN_AF_SIGNAL_SPI_SELECT0] = "SPI_SELECT0",
    [MACHINE_PIN_AF_SIGNAL_SPI_SELECT1] = "SPI_SELECT1",

    [MACHINE_PIN_AF_SIGNAL_UART_TX] = "UART_TX",
    [MACHINE_PIN_AF_SIGNAL_UART_RX] = "UART_RX",
    [MACHINE_PIN_AF_SIGNAL_UART_CTS] = "UART_CTS",
    [MACHINE_PIN_AF_SIGNAL_UART_RTS] = "UART_RTS",

    [MACHINE_PIN_AF_SIGNAL_PDM_CLK] = "PDM_CLK",
    [MACHINE_PIN_AF_SIGNAL_PDM_DATA] = "PDM_DATA",

    /* TODO: Add additional types */
};
