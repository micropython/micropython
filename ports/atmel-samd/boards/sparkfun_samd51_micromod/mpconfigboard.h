// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "SparkFun MicroMod SAMD51 Processor"
#define MICROPY_HW_MCU_NAME "samd51j20"

#define CIRCUITPY_MCU_FAMILY samd51

// Status LED
#define MICROPY_HW_LED_STATUS (&pin_PA23)

// On-board flash
#define SPI_FLASH_MOSI_PIN &pin_PA09
#define SPI_FLASH_MISO_PIN &pin_PA10
#define SPI_FLASH_SCK_PIN  &pin_PA08
#define SPI_FLASH_CS_PIN   &pin_PA11

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PA16)
#define DEFAULT_I2C_BUS_SDA (&pin_PA17)

#define DEFAULT_SPI_BUS_SCK (&pin_PA05)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA04)
#define DEFAULT_SPI_BUS_MISO (&pin_PA06)

#define DEFAULT_UART_BUS_RX (&pin_PB30)
#define DEFAULT_UART_BUS_TX (&pin_PB31)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

// The external flash chip has WP (write-protect) and hold pins we should ignore
#define IGNORE_PIN_PB22
#define IGNORE_PIN_PB23
