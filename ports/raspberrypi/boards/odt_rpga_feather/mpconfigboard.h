// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Oak Dev Tech RPGA Feather"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_LED_STATUS (&pin_GPIO25)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO11)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO10)

#define CIRCUITPY_BOARD_I2C         (2)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO11, .sda = &pin_GPIO10}, \
                                     {.scl = &pin_GPIO9, .sda = &pin_GPIO8}}

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO2)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO3)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO0)

#define DEFAULT_UART_BUS_RX (&pin_GPIO9)
#define DEFAULT_UART_BUS_TX (&pin_GPIO8)
