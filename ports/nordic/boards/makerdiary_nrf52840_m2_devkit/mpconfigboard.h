// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "Makerdiary nRF52840 M.2 Developer Kit"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_HW_LED_STATUS       (&pin_P1_07)

#define CIRCUITPY_RGB_STATUS_INVERTED_PWM
#define CIRCUITPY_RGB_STATUS_R             (&pin_P0_30)
#define CIRCUITPY_RGB_STATUS_G             (&pin_P0_29)
#define CIRCUITPY_RGB_STATUS_B             (&pin_P0_31)

#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(1, 10)
#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(1, 14)
#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(1, 15)
#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(1, 12)
#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(1, 11)
#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(1, 13)

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_UART_BUS_RX         (&pin_P0_15)
#define DEFAULT_UART_BUS_TX         (&pin_P0_16)

#define DEFAULT_I2C_BUS_SCL         (&pin_P1_06)
#define DEFAULT_I2C_BUS_SDA         (&pin_P1_05)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_11)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_12)
#define DEFAULT_SPI_BUS_MISO        (&pin_P1_08)
