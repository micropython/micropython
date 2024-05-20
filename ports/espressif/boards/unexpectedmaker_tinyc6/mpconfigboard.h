// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "TinyC6"
#define MICROPY_HW_MCU_NAME         "ESP32C6"

#define MICROPY_HW_NEOPIXEL         (&pin_GPIO23)
#define CIRCUITPY_STATUS_LED_POWER  (&pin_GPIO22)

#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO7)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO6)

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO19)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO21)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO20)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO17)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO16)
