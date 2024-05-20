// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "BastWiFi"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_LED_STATUS (&pin_GPIO14)
