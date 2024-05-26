// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "CrumpS2"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_APA102_MOSI (&pin_GPIO40)
#define MICROPY_HW_APA102_SCK (&pin_GPIO45)
#define MICROPY_HW_APA102_COUNT (1)
