// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME "Oxocard Connect"
#define MICROPY_HW_MCU_NAME "ESP32"

#define MICROPY_HW_LED_STATUS (&pin_GPIO2)

// clang-format off
#define CIRCUITPY_BOARD_I2C (1)
#define CIRCUITPY_BOARD_I2C_PIN {               \
        { .scl = &pin_GPIO22, .sda = &pin_GPIO21 }}
// clang-format on

// clang-format off
#define CIRCUITPY_BOARD_SPI (2)
#define CIRCUITPY_BOARD_SPI_PIN {                                       \
        { .clock = &pin_GPIO14, .mosi = &pin_GPIO13, .miso = &pin_GPIO12 }, \
        { .clock = &pin_GPIO7,  .mosi = &pin_GPIO5,  .miso = &pin_GPIO8  }}
// clang-format on

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO3)
