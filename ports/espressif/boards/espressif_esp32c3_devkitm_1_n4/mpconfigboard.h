// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME       "ESP32-C3-DevKitM-1"
#define MICROPY_HW_MCU_NAME         "ESP32-C3N4"

// Status LED
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO8)
#define MICROPY_HW_NEOPIXEL_COUNT   (1)

// Default bus pins
#define DEFAULT_UART_BUS_RX         (&pin_GPIO20)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO21)

// Serial over UART
#define CIRCUITPY_CONSOLE_UART_RX               DEFAULT_UART_BUS_RX
#define CIRCUITPY_CONSOLE_UART_TX               DEFAULT_UART_BUS_TX
