// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup

#define MICROPY_HW_BOARD_NAME       "M5STACK STAMP-C3"
#define MICROPY_HW_MCU_NAME         "ESP32-C3FN4"

// Status LED
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO2)

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO21, .rx = &pin_GPIO20}}

// Serial over UART
#define CIRCUITPY_CONSOLE_UART_RX               (&pin_GPIO20)
#define CIRCUITPY_CONSOLE_UART_TX               (&pin_GPIO21)
