// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "LILYGO T-DECK"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO40)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO41)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO38)

#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO8)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO18)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)
