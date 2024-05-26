// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Pimoroni Tiny 2040 (8MB)"
#define MICROPY_HW_MCU_NAME "rp2040"

#define CIRCUITPY_RGB_STATUS_INVERTED_PWM
#define CIRCUITPY_RGB_STATUS_R (&pin_GPIO18)
#define CIRCUITPY_RGB_STATUS_G (&pin_GPIO19)
#define CIRCUITPY_RGB_STATUS_B (&pin_GPIO20)
