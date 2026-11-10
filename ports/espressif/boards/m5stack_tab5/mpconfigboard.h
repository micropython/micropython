// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "M5Stack Tab5"
#define MICROPY_HW_MCU_NAME         "ESP32P4"

// I2C bus for touch, IMU, RTC, power monitor, and GPIO expanders
#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO32, .sda = &pin_GPIO31}}

// UART
#define DEFAULT_UART_BUS_RX         (&pin_GPIO38)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO37)

// SPI on M5-Bus
#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO5)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO18)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO19)

#define CIRCUITPY_ESP32P4_SWAP_LSFS (1)
