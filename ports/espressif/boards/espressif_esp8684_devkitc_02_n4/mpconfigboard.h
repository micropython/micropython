// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ESP8684-DevKitC-02-N4"
#define MICROPY_HW_MCU_NAME         "ESP32C2"

#define CIRCUITPY_RGB_STATUS_INVERTED_PWM
#define CIRCUITPY_RGB_STATUS_R         (&pin_GPIO0)
#define CIRCUITPY_RGB_STATUS_G         (&pin_GPIO1)
#define CIRCUITPY_RGB_STATUS_B         (&pin_GPIO8)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO19)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO20)

#define CIRCUITPY_CONSOLE_UART_RX DEFAULT_UART_BUS_RX
#define CIRCUITPY_CONSOLE_UART_TX DEFAULT_UART_BUS_TX
