// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Gravitech Cucumber MS"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO18)

#define MICROPY_HW_LED_STATUS (&pin_GPIO2)
#define MICROPY_HW_LED_STATUS_INVERTED (1)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO40)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO41)

#define DEFAULT_UART_BUS_TX (&pin_GPIO43)
#define DEFAULT_UART_BUS_RX (&pin_GPIO44)
