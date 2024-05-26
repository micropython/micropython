// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Raspberry Pi Zero 2W"

#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO3)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO2)

#define MICROPY_HW_LED_STATUS       (&pin_GPIO29)

#define CIRCUITPY_CONSOLE_UART_TX   (&pin_GPIO14)
#define CIRCUITPY_CONSOLE_UART_RX   (&pin_GPIO15)
