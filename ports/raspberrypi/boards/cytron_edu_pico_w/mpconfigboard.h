// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Cytron EDU PICO W"
#define MICROPY_HW_MCU_NAME "rp2040"

#define CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL (1)
#define CIRCUITPY_DIGITALIO_HAVE_INVALID_DRIVE_MODE (1)

#define MICROPY_HW_LED_STATUS   (&pin_CYW0)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO5, .sda = &pin_GPIO4}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO18, .mosi = &pin_GPIO19, .miso = &pin_GPIO16}}
