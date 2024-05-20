// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MAKERDIARY_NRF52840_MDK_DONGLE

#define MICROPY_HW_BOARD_NAME       "MakerDiary nRF52840 MDK USB Dongle"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define BOARD_HAS_CRYSTAL 1  // according to the schematic we do

#define CIRCUITPY_RGB_STATUS_INVERTED_PWM
#define CIRCUITPY_RGB_STATUS_R      (&pin_P0_23)
#define CIRCUITPY_RGB_STATUS_G      (&pin_P0_22)
#define CIRCUITPY_RGB_STATUS_B      (&pin_P0_24)
