// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Uri Shaked
// SPDX-FileCopyrightText: Copyright (c) 2021 Benjamin Meisels
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "ARAMCON2 Badge"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_HW_LED_STATUS          (&pin_P1_11)

// Board does not have a 32kHz crystal. It does have a 32MHz crystal.
#define BOARD_HAS_32KHZ_XTAL (0)

#ifdef QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(0, 22)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 20)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(1, 4)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(1, 6)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(1, 0)
#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(1, 2)
#endif

#ifdef SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN &pin_P0_20
#define SPI_FLASH_MISO_PIN &pin_P0_22
#define SPI_FLASH_SCK_PIN &pin_P1_00
#define SPI_FLASH_CS_PIN &pin_P1_02
#endif

#define CIRCUITPY_BOOT_BUTTON (&pin_P0_29)

#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed the left button at start up.")

#define CIRCUITPY_INTERNAL_NVM_SIZE (4096)

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_28)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_03)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_01)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P1_10)
#define DEFAULT_SPI_BUS_MISO        (&pin_P1_09)
