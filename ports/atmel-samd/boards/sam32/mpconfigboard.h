// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "SAM32v26"
#define MICROPY_HW_MCU_NAME "samd51j20"
#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_STATUS   (&pin_PA27)
#define MICROPY_HW_NEOPIXEL     (&pin_PA15)

#define CIRCUITPY_INTERNAL_NVM_SIZE 0
#define CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE (128 * 1024)

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL  (&pin_PA09)
#define DEFAULT_I2C_BUS_SDA  (&pin_PA08)

#define DEFAULT_SPI_BUS_SCK  (&pin_PB13)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB12)
#define DEFAULT_SPI_BUS_MISO (&pin_PB14)

#define DEFAULT_UART_BUS_TX  (&pin_PB16)
#define DEFAULT_UART_BUS_RX  (&pin_PB17)

#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
