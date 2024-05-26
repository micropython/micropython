// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Adafruit FunHouse"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_APA102_MOSI (&pin_GPIO14)
#define MICROPY_HW_APA102_SCK (&pin_GPIO15)
#define MICROPY_HW_APA102_COUNT (5)

#define MICROPY_HW_LED_STATUS (&pin_GPIO37)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO33)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO34)

#define DOUBLE_TAP_PIN (&pin_GPIO38)
