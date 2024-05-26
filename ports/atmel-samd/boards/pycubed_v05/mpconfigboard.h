// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once


#define MICROPY_HW_BOARD_NAME "PyCubedv05"
#define MICROPY_HW_MCU_NAME "samd51j19"
#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_NEOPIXEL   (&pin_PA21)

#define CIRCUITPY_INTERNAL_NVM_SIZE 8192

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

// External flash W25Q80DV
#define EXTERNAL_FLASH_QSPI_DUAL

#define CIRCUITPY_DRIVE_LABEL "PYCUBED"
#define CIRCUITPY_BOOT_COUNTER 1

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL  (&pin_PB13)
#define DEFAULT_I2C_BUS_SDA  (&pin_PB12)

#define DEFAULT_SPI_BUS_SCK  (&pin_PA13)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA12)
#define DEFAULT_SPI_BUS_MISO (&pin_PA14)

#define DEFAULT_UART_BUS_TX  (&pin_PB02)
#define DEFAULT_UART_BUS_RX  (&pin_PB03)

#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
