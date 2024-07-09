// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once


#define MICROPY_HW_BOARD_NAME "Adafruit Trellis M4 Express"
#define MICROPY_HW_MCU_NAME "samd51g19"

#define CIRCUITPY_MCU_FAMILY samd51

// This is for Rev D
#define MICROPY_HW_APA102_MOSI   (&pin_PB03)
#define MICROPY_HW_APA102_SCK    (&pin_PB02)

#define DEFAULT_I2C_BUS_SCL (&pin_PB09)
#define DEFAULT_I2C_BUS_SDA (&pin_PB08)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
