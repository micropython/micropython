// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
// SPDX-FileCopyrightText: Copyright (c) 2021 skieast/Bruce Segal
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME       "AITHinker ESP32-C3S_Kit"
#define MICROPY_HW_MCU_NAME         "ESP32-C3FN4"

// Status LED
#define CIRCUITPY_RGB_STATUS_R      (&pin_GPIO3)
#define CIRCUITPY_RGB_STATUS_G      (&pin_GPIO4)
#define CIRCUITPY_RGB_STATUS_B      (&pin_GPIO5)

// Default bus pins
#define DEFAULT_UART_BUS_RX         (&pin_GPIO20)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO21)

// Serial over UART
#define CIRCUITPY_CONSOLE_UART_RX               DEFAULT_UART_BUS_RX
#define CIRCUITPY_CONSOLE_UART_TX               DEFAULT_UART_BUS_TX
