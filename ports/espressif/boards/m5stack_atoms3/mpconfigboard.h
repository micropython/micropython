// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "M5Stack AtomS3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// #define MICROPY_HW_NEOPIXEL (&pin_GPIO35)

#define CIRCUITPY_BOARD_I2C         (2)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO39, .sda = &pin_GPIO38}, \
                                     {.scl = &pin_GPIO1, .sda = &pin_GPIO2}}

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO17)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO21)
