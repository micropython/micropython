// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Oak Dev Tech PixelWing ESP32S2"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO45)

#define MICROPY_HW_LED_STATUS (&pin_GPIO21)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO34)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO33)
