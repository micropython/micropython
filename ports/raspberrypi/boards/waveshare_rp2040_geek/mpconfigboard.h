// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup
#define MICROPY_HW_BOARD_NAME "Waveshare RP2040-GEEK"
#define MICROPY_HW_MCU_NAME "rp2040"

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO29, .sda = &pin_GPIO28}}

#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO10, .mosi = &pin_GPIO11}, \
                                     {.clock = &pin_GPIO18, .mosi = &pin_GPIO19, .miso = &pin_GPIO20}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO4, .rx = &pin_GPIO5}}
