// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// LEDs
#define MICROPY_HW_LED_STATUS   (&pin_PA17)
#define MICROPY_HW_LED_TX   &pin_PA27
#define MICROPY_HW_LED_RX   &pin_PA17

#define MICROPY_HW_BOARD_NAME "J&J Studios datum-IMU"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define DEFAULT_I2C_BUS_SCL (&pin_PA23)
#define DEFAULT_I2C_BUS_SDA (&pin_PA22)

#define DEFAULT_SPI_BUS_SCK (&pin_PB11)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB10)
#define DEFAULT_SPI_BUS_MISO (&pin_PA12)

#define DEFAULT_UART_BUS_RX (&pin_PA11)
#define DEFAULT_UART_BUS_TX (&pin_PA10)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

// Connected to a crystal
#define IGNORE_PIN_PA00     1
#define IGNORE_PIN_PA01     1

// Unconnected
// Schematic at: https://jandjstudios.io/assets/pdfs/datum-IMU.pdf
#define IGNORE_PIN_PB08     1
#define IGNORE_PIN_PB09     1
#define IGNORE_PIN_PA04     1
#define IGNORE_PIN_PA05     1
#define IGNORE_PIN_PB02     1
#define IGNORE_PIN_PA14     1
#define IGNORE_PIN_PA15     1
#define IGNORE_PIN_PB03     1
#define IGNORE_PIN_PA28     1
#define IGNORE_PIN_PB22     1
#define IGNORE_PIN_PB23     1
#define IGNORE_PIN_PA13     1
