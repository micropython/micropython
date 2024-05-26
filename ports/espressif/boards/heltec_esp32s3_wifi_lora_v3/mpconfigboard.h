// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Heltec ESP32-S3-WIFI-LoRa-V3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_LED_STATUS (&pin_GPIO35) // Pulled from schematic

// I2C
#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO18, .sda = &pin_GPIO17}}

// UART definition for UART connected to the CP210x
#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)

// Serial over UART
#define CIRCUITPY_CONSOLE_UART_RX               DEFAULT_UART_BUS_RX
#define CIRCUITPY_CONSOLE_UART_TX               DEFAULT_UART_BUS_TX
