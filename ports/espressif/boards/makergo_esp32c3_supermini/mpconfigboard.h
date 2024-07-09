// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup

#define MICROPY_HW_BOARD_NAME       "Maker Go ESP32C3 Supermini"
#define MICROPY_HW_MCU_NAME         "ESP32-C3"

// Status LED
#define MICROPY_HW_LED_STATUS (&pin_GPIO8)

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO21, .rx = &pin_GPIO20}}

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO9, .sda = &pin_GPIO8}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO4, .mosi = &pin_GPIO6, .miso = &pin_GPIO5}}

// Reduce wifi.radio.tx_power due to the antenna design of this board
#define CIRCUITPY_WIFI_DEFAULT_TX_POWER   (11.0)
