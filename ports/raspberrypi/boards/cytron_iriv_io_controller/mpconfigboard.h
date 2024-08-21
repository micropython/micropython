// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Wai Weng for Cytron Technologies
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME "Cytron IRIV IO Controller"
#define MICROPY_HW_MCU_NAME "rp2350a"

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO17)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO16)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO22)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO19)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO20)

#define DEFAULT_UART_BUS_RX (&pin_GPIO25)
#define DEFAULT_UART_BUS_TX (&pin_GPIO24)
