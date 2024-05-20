// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define ELECTRONUT_LABS_PAPYR

#define MICROPY_HW_BOARD_NAME       "Electronut Labs Papyr"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_06)
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_05)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_31)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_29)
#define DEFAULT_SPI_BUS_MISO        (&pin_P1_01)

#define DEFAULT_UART_BUS_RX         (&pin_P0_07)
#define DEFAULT_UART_BUS_TX         (&pin_P0_08)
