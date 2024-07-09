// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Neradoc
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME       "Spotpear ESP32C3 LCD 1.44"
#define MICROPY_HW_MCU_NAME         "ESP32-C3FH4"

#define MICROPY_HW_LED_STATUS       (&pin_GPIO11)
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO9)
