// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Benjamin Shockley
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Waveshare ESP32-C6 LCD 1.47"
#define MICROPY_HW_MCU_NAME         "ESP32-C6FH4"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO8)
#define MICROPY_HW_NEOPIXEL_ORDER (STATUS_NEOPIXEL_ORDER_RGB)

// I2C
#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO0, .sda = &pin_GPIO1}}

// SPI
#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO7, .mosi = &pin_GPIO6, .miso = &pin_GPIO5}}

// TXD0 and RXD0
#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO16, .rx = &pin_GPIO17}}

// For entering safe mode, use BOOT button
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO9)

// Explanation of how a user got into safe mode
#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed the BOOT button at start up.")

// Reduce wifi.radio.tx_power due to the antenna design of this board
#define CIRCUITPY_WIFI_DEFAULT_TX_POWER   (15)
