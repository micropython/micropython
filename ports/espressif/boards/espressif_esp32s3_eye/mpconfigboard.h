// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ESP32-S3-EYE"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// Shared by the camera and accelerometer
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO4)
#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO5)

// This is the SD card connection, not the LCD
#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO39)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO40)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO38)

#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     { \
        {.clock = &pin_GPIO21, .mosi = &pin_GPIO47, .miso = NULL}, \
        {.clock = &pin_GPIO39, .mosi = &pin_GPIO40, .miso = &pin_GPIO38}, \
}
