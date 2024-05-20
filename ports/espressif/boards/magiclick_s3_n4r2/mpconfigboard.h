// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "MagiClick S3 N4R2"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO17)
// #define CIRCUITPY_STATUS_LED_POWER (&pin_GPIO38)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO36, .sda = &pin_GPIO35}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO5, .mosi = &pin_GPIO4, .miso = &pin_GPIO6}}

// #define CIRCUITPY_BOARD_UART        (1)
// #define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO5, .rx = &pin_GPIO16}}

#define DOUBLE_TAP_PIN (&pin_GPIO6)
