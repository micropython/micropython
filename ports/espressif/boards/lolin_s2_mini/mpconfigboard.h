// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "S2Mini"
#define MICROPY_HW_MCU_NAME         "ESP32S2-S2FN4R2" // from Wemos MP

#define MICROPY_HW_LED_STATUS (&pin_GPIO15)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO35) // no I2C labels on S2 Mini, def from Wemos MP
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO33) // no I2C labels on S2 Mini, def from Wemos MP

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO7)   // no SPI labels on S2 Mini, def from Wemos MP
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11) // no SPI labels on S2 Mini, def from Wemos MP
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO9)  // no SPI labels on S2 Mini, def from Wemos MP
