// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ESP32-S2-DevKitC-1-N8R2"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_NEOPIXEL         (&pin_GPIO18)

#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)
