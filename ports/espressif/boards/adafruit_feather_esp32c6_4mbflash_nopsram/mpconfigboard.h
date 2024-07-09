// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Adafruit Feather ESP32-C6 4MB Flash No PSRAM"
#define MICROPY_HW_MCU_NAME         "ESP32C6"

// Don't use the neopixel for status because we can't use it at the same time as
// the boot button.
// #define MICROPY_HW_NEOPIXEL (&pin_GPIO9)

#define MICROPY_HW_LED_STATUS (&pin_GPIO15)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO18, .sda = &pin_GPIO19}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO21, .mosi = &pin_GPIO22, .miso = &pin_GPIO23}}

// TXD0 and RXD0
#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO16, .rx = &pin_GPIO17}}

// For entering safe mode, use BOOT button
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO9)

// Explanation of how a user got into safe mode
#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed the BOOT button at start up.")
