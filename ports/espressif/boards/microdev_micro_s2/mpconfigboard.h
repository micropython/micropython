// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME       "MicroDev microS2"
#define MICROPY_HW_MCU_NAME         "ESP32-S2"

// Status LEDs
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO33)

#define MICROPY_HW_LED_STATUS (&pin_GPIO21)

// Default bus pins
#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO1)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO2)

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO36)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO35)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO37)

#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)
#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
