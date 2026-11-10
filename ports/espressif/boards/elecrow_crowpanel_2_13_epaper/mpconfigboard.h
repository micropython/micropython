// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Cade Roux
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "CrowPanel 2.13 EPaper"
#define MICROPY_HW_MCU_NAME         "ESP32S3"
#define MICROPY_HW_LED_STATUS       (&pin_GPIO19)
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     { \
        {.clock = &pin_GPIO12, .mosi = &pin_GPIO11}, /*EPD*/ \
}

// UART pins attached to the USB-serial converter chip
#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)

#define CIRCUITPY_CONSOLE_UART_TX DEFAULT_UART_BUS_TX
#define CIRCUITPY_CONSOLE_UART_RX DEFAULT_UART_BUS_RX
