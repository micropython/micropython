// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Adafruit P4 GPIO"
#define MICROPY_HW_MCU_NAME         "ESP32P4"

#define MICROPY_HW_NEOPIXEL         (&pin_GPIO52)

#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO35)

#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO54)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO53)

// FS USB on GPIO24/25 routed to J2.
#define CIRCUITPY_USB_HOST_INSTANCE 0
