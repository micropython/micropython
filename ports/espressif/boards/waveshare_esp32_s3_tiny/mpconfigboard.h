// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Waveshare ESP32-S3-Tiny"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// This corrects the color ordering so that the CircuitPython status lights behave as expected
#define MICROPY_HW_NEOPIXEL_ORDER (STATUS_NEOPIXEL_ORDER_RGB)
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO38)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO19)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO20)

// Reduce wifi.radio.tx_power due to the antenna design of this board
#define CIRCUITPY_WIFI_DEFAULT_TX_POWER   (15)
