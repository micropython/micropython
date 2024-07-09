// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "HEIA-FR Picomo V2"
#define MICROPY_HW_MCU_NAME "rp2040"

#define CIRCUITPY_RGB_STATUS_R (&pin_GPIO10)
#define CIRCUITPY_RGB_STATUS_G (&pin_GPIO9)
#define CIRCUITPY_RGB_STATUS_B (&pin_GPIO8)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO21)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO20)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO18)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO19)
