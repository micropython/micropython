// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME       "MoonYard Equinox"
#define MICROPY_HW_MCU_NAME         "rp2350b"

#define MICROPY_HW_LED_STATUS       (&pin_GPIO18)

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO14)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO12)

#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO4)
#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO5)
