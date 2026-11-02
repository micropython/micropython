// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Adafruit PyPortal"
#define MICROPY_HW_MCU_NAME "samd51j20"

#define CIRCUITPY_MCU_FAMILY samd51

// This is for Rev B

#define MICROPY_HW_LED_STATUS   (&pin_PA27)

#define MICROPY_HW_NEOPIXEL (&pin_PB22)

#define DEFAULT_I2C_BUS_SCL (&pin_PB03)
#define DEFAULT_I2C_BUS_SDA (&pin_PB02)

#define DEFAULT_SPI_BUS_SCK (&pin_PA13)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA12)
#define DEFAULT_SPI_BUS_MISO (&pin_PA14)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

// The ESP32 running nina-fw, on board.SPI().
#define CIRCUITPY_AIRLIFT_CS (&pin_PB14)
#define CIRCUITPY_AIRLIFT_BUSY (&pin_PB16)
#define CIRCUITPY_AIRLIFT_RESET (&pin_PB17)
#define CIRCUITPY_AIRLIFT_GPIO0 (&pin_PB15)
