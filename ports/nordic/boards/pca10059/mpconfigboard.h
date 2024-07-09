// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "PCA10059 nRF52840 Dongle"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_HW_LED_STATUS          (&pin_P0_06)
#define CIRCUITPY_RGB_STATUS_INVERTED_PWM
#define CIRCUITPY_RGB_STATUS_R      (&pin_P0_08)
#define CIRCUITPY_RGB_STATUS_G      (&pin_P1_09)
#define CIRCUITPY_RGB_STATUS_B      (&pin_P0_12)
