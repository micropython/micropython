// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Raspberry Breadstick"
#define MICROPY_HW_MCU_NAME "rp2040"


#define MICROPY_HW_APA102_MOSI (&pin_GPIO17)
#define MICROPY_HW_APA102_SCK (&pin_GPIO16)
#define MICROPY_HW_APA102_COUNT (24)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO23)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO22)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO10)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO8)

#define DEFAULT_UART_BUS_RX (&pin_GPIO21)
#define DEFAULT_UART_BUS_TX (&pin_GPIO20)
