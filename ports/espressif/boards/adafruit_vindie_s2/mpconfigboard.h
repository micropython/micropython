// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Adafruit Vindie S2"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO33)
#define MICROPY_HW_NEOPIXEL_COUNT   (4)

#define MICROPY_HW_LED_STATUS (&pin_GPIO18)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO11)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO12)

#define DEFAULT_UART_BUS_RX (&pin_GPIO21)
#define DEFAULT_UART_BUS_TX (&pin_GPIO45)

#define DOUBLE_TAP_PIN (&pin_GPIO17)
