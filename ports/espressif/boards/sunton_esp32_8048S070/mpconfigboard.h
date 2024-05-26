// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Sunton-ESP32-8048S070"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define DEFAULT_I2C_BUS_SDA (&pin_GPIO19)
#define DEFAULT_I2C_BUS_SCL (&pin_GPIO20)

#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11)
#define DEFAULT_SPI_BUS_SCK (&pin_GPIO12)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO13)

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO43)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO44)
