// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 CDarius
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "M5Stack Core2"
#define MICROPY_HW_MCU_NAME         "ESP32"

#define CIRCUITPY_BOARD_I2C         (2)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO22, .sda = &pin_GPIO21}, \
                                     {.scl = &pin_GPIO33, .sda = &pin_GPIO32}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO18, .mosi = &pin_GPIO23, .miso = &pin_GPIO38}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO14, .rx = &pin_GPIO13}}

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO3)
