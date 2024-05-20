// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "DynOSSAT-EDU-OBC"
#define MICROPY_HW_MCU_NAME "samd51j20"

#define MICROPY_HW_NEOPIXEL (&pin_PA08)

#define SPI_FLASH_MOSI_PIN          &pin_PA16
#define SPI_FLASH_MISO_PIN          &pin_PA18
#define SPI_FLASH_SCK_PIN           &pin_PA17
#define SPI_FLASH_CS_PIN            &pin_PA19

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PB13)
#define DEFAULT_I2C_BUS_SDA (&pin_PB12)

#define DEFAULT_SPI_BUS_SCK (&pin_PB03)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB02)
#define DEFAULT_SPI_BUS_MISO (&pin_PB01)

#define DEFAULT_UART_BUS_RX (&pin_PA23)
#define DEFAULT_UART_BUS_TX (&pin_PA22)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
#define IGNORE_PIN_PA13     1
#define IGNORE_PIN_PA14     1
#define IGNORE_PIN_PA27     1
#define IGNORE_PIN_PB04     1
#define IGNORE_PIN_PB05     1
#define IGNORE_PIN_PB17     1
#define IGNORE_PIN_PB23     1
#define IGNORE_PIN_PB31     1
