// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "CP32-M4"
#define MICROPY_HW_MCU_NAME "samd51j20"

#define MICROPY_HW_APA102_MOSI   (&pin_PA12)
#define MICROPY_HW_APA102_SCK    (&pin_PA13)

#define CIRCUITPY_MCU_FAMILY samd51

#define DEFAULT_I2C_BUS_SCL (&pin_PB09)
#define DEFAULT_I2C_BUS_SDA (&pin_PB08)

#define DEFAULT_SPI_BUS_SCK (&pin_PA22)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA23)
#define DEFAULT_SPI_BUS_MISO (&pin_PA21)

#define DEFAULT_UART_BUS_RX (&pin_PB17)
#define DEFAULT_UART_BUS_TX (&pin_PB16)
