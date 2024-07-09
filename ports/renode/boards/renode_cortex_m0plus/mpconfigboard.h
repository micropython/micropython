// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Renode Cortex-M0+"
#define MICROPY_HW_MCU_NAME "cortex-m0+"

#define DEFAULT_UART_BUS_RX (&pin_GPIO1)
#define DEFAULT_UART_BUS_TX (&pin_GPIO0)

#define CIRCUITPY_CONSOLE_UART_TX DEFAULT_UART_BUS_TX
#define CIRCUITPY_CONSOLE_UART_RX DEFAULT_UART_BUS_RX
