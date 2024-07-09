// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Pontus Oldberg, Invector Labs
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Challenger RP2040 LoRa"
#define MICROPY_HW_MCU_NAME "rp2040"

#define DEFAULT_UART_BUS_TX   (&pin_GPIO16)
#define DEFAULT_UART_BUS_RX   (&pin_GPIO17)
#define DEFAULT_I2C_BUS_SDA   (&pin_GPIO0)
#define DEFAULT_I2C_BUS_SCL   (&pin_GPIO1)
#define DEFAULT_SPI_BUS_SCK   (&pin_GPIO22)
#define DEFAULT_SPI_BUS_MOSI  (&pin_GPIO23)
#define DEFAULT_SPI_BUS_MISO  (&pin_GPIO20)
