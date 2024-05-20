// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Seeed Studio XIAO ESP32C3"
#define MICROPY_HW_MCU_NAME "ESP32-C3FN4"

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO7)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO6)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO8)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO10)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO9)

#define DEFAULT_UART_BUS_RX (&pin_GPIO20)
#define DEFAULT_UART_BUS_TX (&pin_GPIO21)
