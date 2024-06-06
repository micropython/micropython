// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Bill Sideris, independently providing these changes.
//
// SPDX-License-Identifier: MIT

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "NodeMcu-ESP32-C2"
#define MICROPY_HW_MCU_NAME         "ESP32C2"

#define DEFAULT_UART_BUS_RX         (&pin_GPIO19)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO20)

#define CIRCUITPY_CONSOLE_UART_RX DEFAULT_UART_BUS_RX
#define CIRCUITPY_CONSOLE_UART_TX DEFAULT_UART_BUS_TX
