// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "SPRESENSE"
#define MICROPY_HW_MCU_NAME "CXD5602"

#define DEFAULT_I2C_BUS_SCL (&pin_I2C0_BCK)
#define DEFAULT_I2C_BUS_SDA (&pin_I2C0_BDT)

#define DEFAULT_SPI_BUS_SCK (&pin_SPI4_SCK)
#define DEFAULT_SPI_BUS_MISO (&pin_SPI4_MISO)
#define DEFAULT_SPI_BUS_MOSI (&pin_SPI4_MOSI)

#define DEFAULT_UART_BUS_RX (&pin_UART2_RXD)
#define DEFAULT_UART_BUS_TX (&pin_UART2_TXD)

#define MICROPY_PY_REVERSE_SPECIAL_METHODS          (1)
