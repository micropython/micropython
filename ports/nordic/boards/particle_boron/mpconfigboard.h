// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "Particle Boron"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define CIRCUITPY_RGB_STATUS_R       (&pin_P0_13)
#define CIRCUITPY_RGB_STATUS_G       (&pin_P0_14)
#define CIRCUITPY_RGB_STATUS_B       (&pin_P0_15)

#if QSPI_FLASH_FILESYSTEM
#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(0, 20)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 21)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(0, 22)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(0, 23)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(0, 19)
#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(0, 17)
#endif

#if SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN &pin_P0_20
#define SPI_FLASH_MISO_PIN &pin_P0_21
#define SPI_FLASH_SCK_PIN &pin_P0_19
#define SPI_FLASH_CS_PIN &pin_P0_17
#endif

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_27)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_26)

#define DEFAULT_SPI_BUS_SCK         (&pin_P1_15)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P1_13)
#define DEFAULT_SPI_BUS_MISO        (&pin_P1_14)

#define DEFAULT_UART_BUS_RX         (&pin_P0_08)
#define DEFAULT_UART_BUS_TX         (&pin_P0_06)
