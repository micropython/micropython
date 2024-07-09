// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "BDMICRO VINA-D51"
#define MICROPY_HW_MCU_NAME "samd51n20"

#define CIRCUITPY_MCU_FAMILY samd51

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PA16)
#define DEFAULT_I2C_BUS_SDA (&pin_PA17)
#define DEFAULT_UART_BUS_RX (&pin_PB20)
#define DEFAULT_UART_BUS_TX (&pin_PB21)
#define DEFAULT_SPI_BUS_MISO (&pin_PB23)
#define DEFAULT_SPI_BUS_MOSI (&pin_PC27)
#define DEFAULT_SPI_BUS_SCK (&pin_PC28)
#define MICROPY_HW_LED_STATUS (&pin_PA23)
#define MICROPY_HW_LED_RX (&pin_PC05)
#define MICROPY_HW_LED_TX (&pin_PC06)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24 1
#define IGNORE_PIN_PA25 1
