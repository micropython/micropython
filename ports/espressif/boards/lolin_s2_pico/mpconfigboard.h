// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "S2Pico"
#define MICROPY_HW_MCU_NAME         "ESP32S2-S2FN4R2" // from Wemos MP

#define MICROPY_HW_LED_STATUS (&pin_GPIO10)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO9) // JST SH Connector Pin 3 NOT STEMMA QT / Feather pinout
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO8) // JST SH Connector Pin 2 NOT STEMMA QT / Feather pinout

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO36)   // no SPI labels on S2 Pico, def from schematic
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO35) // no SPI labels on S2 Pico, def from schematic
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO37)  // no SPI labels on S2 Pico, def from schematic
