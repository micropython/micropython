// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Waveshare ESP32S3 LCD 1.28"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define DEFAULT_I2C_BUS_SDA (&pin_GPIO6)
#define DEFAULT_I2C_BUS_SCL (&pin_GPIO7)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO10)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO13)

#define DEFAULT_UART_BUS_RX (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX (&pin_GPIO43)

#define CIRCUITPY_CONSOLE_UART_RX DEFAULT_UART_BUS_RX
#define CIRCUITPY_CONSOLE_UART_TX DEFAULT_UART_BUS_TX
