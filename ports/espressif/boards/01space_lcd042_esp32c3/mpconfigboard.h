// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Neradoc
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME       "01Space 0.42 OLED ESP32C3"
#define MICROPY_HW_MCU_NAME         "ESP32-C3FH4"

// Status LED
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO2)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO6, .sda = &pin_GPIO5}}

// For entering safe mode
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO9)
