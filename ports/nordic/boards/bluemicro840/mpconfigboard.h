// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2021 Pierre Constantineau
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "BlueMicro840"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define BOARD_HAS_CRYSTAL 1

#define MICROPY_HW_LED_STATUS       (&pin_P1_04)  // RED

#define DEFAULT_I2C_BUS_SCL         (&pin_P0_17)  // 0.17 - same position as Pro Micro
#define DEFAULT_I2C_BUS_SDA         (&pin_P0_15)  // 0.15 - same position as Pro Micro

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_24)  // 0.24 - same position as Pro Micro
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_10)  // 0.10 - same position as Pro Micro
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_09)  // 0.09 - same position as Pro Micro

#define DEFAULT_UART_BUS_RX         (&pin_P0_08)  // 0.08 - same position as Pro Micro
#define DEFAULT_UART_BUS_TX         (&pin_P0_06)  // 0.06 - same position as Pro Micro
