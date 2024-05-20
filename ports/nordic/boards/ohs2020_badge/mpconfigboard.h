// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2020 Michael Welling
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "Open Hardware Summit 2020 Badge"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#if QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(1,  0)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 22)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(0, 20)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(1,  2)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(1,  1)
#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(0, 23)
#endif

#if SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN &pin_P1_00
#define SPI_FLASH_MISO_PIN &pin_P0_22
#define SPI_FLASH_SCK_PIN &pin_P1_01
#define SPI_FLASH_CS_PIN &pin_P0_23
#endif

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL         (&pin_P1_14)
#define DEFAULT_I2C_BUS_SDA         (&pin_P1_15)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_11)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_12)
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_07)
