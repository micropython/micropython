// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Adafruit ItsyBitsy ESP32"
#define MICROPY_HW_MCU_NAME         "ESP32"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO0)
#define CIRCUITPY_STATUS_LED_POWER (&pin_GPIO2)

#define MICROPY_HW_LED_STATUS (&pin_GPIO13)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO27, .sda = &pin_GPIO15}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO19, .mosi = &pin_GPIO21, .miso = &pin_GPIO22}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO20, .rx = &pin_GPIO8}}

#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO35)

// Explanation of how a user got into safe mode
#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed the BOOT button at start up.")

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO3)
