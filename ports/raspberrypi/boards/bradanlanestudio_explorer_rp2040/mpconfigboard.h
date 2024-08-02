// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Bradán Lane STUDIO
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME   "Bradán Lane STUDIO Explorer Badge"
#define MICROPY_HW_MCU_NAME     "rp2040"

#define MICROPY_HW_LED_STATUS   (&pin_GPIO4)
// the UART pins are used for IR
// #define DEFAULT_UART_BUS_TX     (&pin_GPIO0)
// #define DEFAULT_UART_BUS_RX     (&pin_GPIO1)
#define DEFAULT_I2C_BUS_SDA     (&pin_GPIO2)
#define DEFAULT_I2C_BUS_SCL     (&pin_GPIO3)

#define DEFAULT_SPI_BUS_BUSY    (&pin_GPIO9)
#define DEFAULT_SPI_BUS_RESET   (&pin_GPIO10)
#define DEFAULT_SPI_BUS_DC      (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO    (&pin_GPIO12)
#define DEFAULT_SPI_BUS_CS      (&pin_GPIO13)
#define DEFAULT_SPI_BUS_SCK     (&pin_GPIO14)
#define DEFAULT_SPI_BUS_MOSI    (&pin_GPIO15)
