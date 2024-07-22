// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "VIDI X"
#define MICROPY_HW_MCU_NAME         "ESP32"

#define MICROPY_HW_LED_STATUS (&pin_GPIO2)

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO18, .mosi = &pin_GPIO23, .miso = &pin_GPIO19}}

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO32, .sda = &pin_GPIO33}}

#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)

// Explanation of how a user got into safe mode
#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed the VOLUME button at start up.")

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO3)
