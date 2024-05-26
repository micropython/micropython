// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Maker badge by Czech maker"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO18)
#define MICROPY_HW_NEOPIXEL_COUNT (4)

#define CIRCUITPY_BOOT_BUTTON (&pin_GPIO0)

#define AUTORESET_DELAY_MS 500

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO9)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO8)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO36)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO35)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO37)

#define DEFAULT_UART_BUS_RX (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX (&pin_GPIO43)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_GPIO18     1
#define IGNORE_PIN_GPIO19     1

#define DOUBLE_TAP_PIN (&pin_GPIO38)
