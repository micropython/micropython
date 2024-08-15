// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit
// Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME "RGBTouch Mini"
#define MICROPY_HW_MCU_NAME "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO39)
#define CIRCUITPY_STATUS_LED_POWER (&pin_GPIO38)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO9)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO8)
