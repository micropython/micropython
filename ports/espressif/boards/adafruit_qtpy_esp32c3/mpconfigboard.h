// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
// SPDX-FileCopyrightText: Copyright (c) 2021 skieast/Bruce Segal
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME       "Adafruit QT Py ESP32C3"
#define MICROPY_HW_MCU_NAME         "ESP32-C3FN4"

// Status LED
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO2)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO6, .sda = &pin_GPIO5}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO10, .mosi = &pin_GPIO7, .miso = &pin_GPIO8}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO21, .rx = &pin_GPIO20}}

// For entering safe mode
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO9)

// Reduce wifi.radio.tx_power due to the antenna design of this board
#define CIRCUITPY_WIFI_DEFAULT_TX_POWER   (15)
