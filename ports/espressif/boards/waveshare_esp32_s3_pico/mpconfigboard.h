// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Waveshare ESP32-S3-Pico"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL         (&pin_GPIO21)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO12)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO11)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO18)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO17)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO36)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO35)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO37)
