// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Newsan SA
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME "Archi RP2040"
#define MICROPY_HW_MCU_NAME "rp2040"


#define DEFAULT_I2C_BUS_SCL (&pin_GPIO3)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO2)

#define DEFAULT_UART_BUS_TX (&pin_GPIO8)
#define DEFAULT_UART_BUS_RX (&pin_GPIO9)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO6)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO7)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO4)

#define MICROPY_HW_NEOPIXEL         (&pin_GPIO24)
#define MICROPY_HW_NEOPIXEL_COUNT   (64)
