// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Adafruit Feather ESP32 V2"
#define MICROPY_HW_MCU_NAME         "ESP32"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO0)
#define CIRCUITPY_STATUS_LED_POWER (&pin_GPIO2)

#define MICROPY_HW_LED_STATUS (&pin_GPIO13)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO20, .sda = &pin_GPIO22}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO5, .mosi = &pin_GPIO19, .miso = &pin_GPIO21}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO8, .rx = &pin_GPIO7}}

// For entering safe mode, use SW38 button
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO38)

// Explanation of how a user got into safe mode
#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed the SW38 button at start up.")

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO3)
