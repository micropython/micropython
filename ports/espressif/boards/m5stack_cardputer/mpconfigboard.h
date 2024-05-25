// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "M5Stack Cardputer"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO21)
#define DEFAULT_I2C_BUS_SCL (&pin_GPIO1)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO2)
#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO36, .mosi = &pin_GPIO35}, \
                                     {.clock = &pin_GPIO40, .mosi = &pin_GPIO14, .miso = &pin_GPIO39}}
