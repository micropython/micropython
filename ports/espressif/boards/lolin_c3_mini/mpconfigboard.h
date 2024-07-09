// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup

#define MICROPY_HW_BOARD_NAME       "Wemos Lolin C3 Mini" // from Wemos MP
#define MICROPY_HW_MCU_NAME         "ESP32-C3FH4" // from Wemos MP

// From Wemos C3 Mini Schematic
// https://www.wemos.cc/en/latest/_static/files/sch_c3_mini_v1.0.0.pdf
// And MP Config
// https://github.com/micropython/micropython/blob/master/ports/esp32/boards/LOLIN_C3_MINI
// Status LED
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO7)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO6, .sda = &pin_GPIO5}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO10, .mosi = &pin_GPIO7, .miso = &pin_GPIO8}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO21, .rx = &pin_GPIO20}}

// Reduce wifi.radio.tx_power due to the antenna design of this board
#define CIRCUITPY_WIFI_DEFAULT_TX_POWER   (11.0)
