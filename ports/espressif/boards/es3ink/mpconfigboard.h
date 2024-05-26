// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ES3ink"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL         (&pin_GPIO8)
#define CIRCUITPY_STATUS_LED_POWER  (&pin_GPIO18)

#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)

#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO3)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO4)

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO12)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO13)

#define DOUBLE_TAP_PIN              (&pin_GPIO38)
