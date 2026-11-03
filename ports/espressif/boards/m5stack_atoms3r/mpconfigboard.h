// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Cade Roux
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "M5Stack AtomS3R"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// board.I2C(): the internal bus (LP5562 backlight driver at 0x30, BMI270 IMU at 0x68).
// board.PORTA_I2C(): the Grove port.
#define CIRCUITPY_BOARD_I2C         (2)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO0, .sda = &pin_GPIO45}, \
                                     {.scl = &pin_GPIO1, .sda = &pin_GPIO2}}

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO15)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO21)
