// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "M5Stack CoreS3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define CIRCUITPY_BOARD_I2C         (2)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO11, .sda = &pin_GPIO12}, \
                                     {.scl = &pin_GPIO1, .sda = &pin_GPIO2}}

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO36)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO37)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO35)

#define DEFAULT_UART_BUS_RX (&pin_GPIO18)
#define DEFAULT_UART_BUS_TX (&pin_GPIO17)
