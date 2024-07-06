// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 CDarius
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Waveshare RP2040-PiZero"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_LED_STATUS (&pin_GPIO25)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO3)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO2)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO10)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO12)

#define DEFAULT_UART_BUS_RX (&pin_GPIO5)
#define DEFAULT_UART_BUS_TX (&pin_GPIO4)
