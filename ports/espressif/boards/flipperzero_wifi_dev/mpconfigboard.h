// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Flipper Zero Wi-Fi Dev"
#define MICROPY_HW_MCU_NAME         "ESP32S2"

#define CIRCUITPY_RGB_STATUS_INVERTED_PWM
#define CIRCUITPY_RGB_STATUS_R (&pin_GPIO6)
#define CIRCUITPY_RGB_STATUS_G (&pin_GPIO5)
#define CIRCUITPY_RGB_STATUS_B (&pin_GPIO4)

#define MICROPY_HW_LED_STATUS   (&pin_GPIO5) // Green LED

#define DEFAULT_I2C_BUS_SCL  (&pin_GPIO1)
#define DEFAULT_I2C_BUS_SDA  (&pin_GPIO2)

#define DEFAULT_SPI_BUS_SCK  (&pin_GPIO12)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO13)

#define DEFAULT_UART_BUS_RX  (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX  (&pin_GPIO43)
