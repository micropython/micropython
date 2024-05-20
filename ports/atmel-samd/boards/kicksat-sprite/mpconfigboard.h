// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once


#define MICROPY_HW_BOARD_NAME "Sprite_v2b"
#define MICROPY_HW_MCU_NAME "samd51G19"
#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_STATUS   (&pin_PB03)

#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define DEFAULT_I2C_BUS_SCL  (&pin_PA17)
#define DEFAULT_I2C_BUS_SDA  (&pin_PA16)

#define DEFAULT_SPI_BUS_SCK  (&pin_PA05)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA07)
#define DEFAULT_SPI_BUS_MISO (&pin_PA04)

#define DEFAULT_UART_BUS_TX  (&pin_PB09)
#define DEFAULT_UART_BUS_RX  (&pin_PB08)

#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

#define MICROPY_FATFS_EXFAT 0
