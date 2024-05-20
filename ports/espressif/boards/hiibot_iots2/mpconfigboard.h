// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "IoTs2"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_LED (&pin_GPIO37)
#define MICROPY_HW_NEOPIXEL (&pin_GPIO16)

#define MICROPY_HW_BUTTON (&pin_GPIO21)
#define CIRCUITPY_BOOT_BUTTON (&pin_GPIO21)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO1, .sda = &pin_GPIO2}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO40, .mosi = &pin_GPIO42, .miso = &pin_GPIO41}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO43, .rx = &pin_GPIO44}}
