// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Bill Sideris, independently providing these changes.
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "M5Stack Timer Camera X"
#define MICROPY_HW_MCU_NAME         "ESP32"

#define MICROPY_HW_LED_STATUS       (&pin_GPIO2)

#define CIRCUITPY_BOARD_I2C         (3)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO13, .sda = &pin_GPIO4}, \
                                     {.scl = &pin_GPIO14, .sda = &pin_GPIO12}, \
                                     {.scl = &pin_GPIO23, .sda = &pin_GPIO25}}

// Ext. Port
// BM8563
// Camera sensor

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO3)

#define CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP (1)
