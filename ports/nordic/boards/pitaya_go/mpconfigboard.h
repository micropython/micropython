// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2020 Yihui Xiong
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MAKERDIARYPITAYAGO

#define MICROPY_HW_BOARD_NAME       "Makerdiary Pitaya Go"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_QSPI_DATA0          NRF_GPIO_PIN_MAP(1, 6)
#define MICROPY_QSPI_DATA1          NRF_GPIO_PIN_MAP(1, 1)
#define MICROPY_QSPI_DATA2          NRF_GPIO_PIN_MAP(1, 5)
#define MICROPY_QSPI_DATA3          NRF_GPIO_PIN_MAP(1, 2)
#define MICROPY_QSPI_SCK            NRF_GPIO_PIN_MAP(1, 4)
#define MICROPY_QSPI_CS             NRF_GPIO_PIN_MAP(1, 3)

#define BOARD_HAS_CRYSTAL           1
