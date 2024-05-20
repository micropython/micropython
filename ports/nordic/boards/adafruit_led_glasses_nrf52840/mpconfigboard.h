// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "Adafruit LED Glasses Driver nRF52840"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_HW_NEOPIXEL         (&pin_P1_15)

#define MICROPY_HW_LED_STATUS       (&pin_P0_31)

#if QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(1, 00)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 21)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(0, 22)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(0, 23)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(0, 19)
#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(0, 20)
#endif

#if SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN &pin_P1_00
#define SPI_FLASH_MISO_PIN &pin_P0_21
#define SPI_FLASH_SCK_PIN &pin_P0_19
#define SPI_FLASH_CS_PIN &pin_P0_20
#endif

// Board does not have a 32kHz crystal. It does have a 32MHz crystal, in the module.
#define BOARD_HAS_32KHZ_XTAL (0)

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_08)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_06)
