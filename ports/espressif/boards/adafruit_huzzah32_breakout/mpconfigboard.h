// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Adafruit HUZZAH32 Breakout"
#define MICROPY_HW_MCU_NAME         "ESP32"

#define MICROPY_HW_LED_STATUS (&pin_GPIO13)

// For entering safe mode, use GPIO0 button
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)

// Explanation of how a user got into safe mode
#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed the GPIO0 button at start up.")

// UART pins
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO3)
