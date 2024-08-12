// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Bill Sideris, independently providing these changes
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "LILYGO T-Watch-S3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define CIRCUITPY_BOARD_I2C         (2)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO11, .sda = &pin_GPIO10}, \
                                     {.scl = &pin_GPIO40, .sda = &pin_GPIO39}}

#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO18, .mosi = &pin_GPIO13}, \
                                     {.clock = &pin_GPIO3, .mosi = &pin_GPIO1, .miso = &pin_GPIO4}}
// LCD then LORA

#define DOUBLE_TAP_PIN (&pin_GPIO21)
