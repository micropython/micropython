// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME "Pimoroni Tiny 2350"
#define MICROPY_HW_MCU_NAME "rp2350a"

#define CIRCUITPY_RGB_STATUS_INVERTED_PWM
#define CIRCUITPY_RGB_STATUS_R (&pin_GPIO18)
#define CIRCUITPY_RGB_STATUS_G (&pin_GPIO19)
#define CIRCUITPY_RGB_STATUS_B (&pin_GPIO20)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO13, .sda = &pin_GPIO12}}
