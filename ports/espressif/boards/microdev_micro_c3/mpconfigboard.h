// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME           "MicroDev microC3"
#define MICROPY_HW_MCU_NAME             "ESP32-C3FN4"

// Status LEDs
#define MICROPY_HW_NEOPIXEL             (&pin_GPIO7)
#define MICROPY_HW_NEOPIXEL_COUNT       (2)

#define MICROPY_HW_LED_STATUS           (&pin_GPIO8)

// Default bus pins
#define DEFAULT_I2C_BUS_SCL             (&pin_GPIO4)
#define DEFAULT_I2C_BUS_SDA             (&pin_GPIO5)

#define DEFAULT_SPI_BUS_SCK             (&pin_GPIO1)
#define DEFAULT_SPI_BUS_MOSI            (&pin_GPIO2)
#define DEFAULT_SPI_BUS_MISO            (&pin_GPIO3)

#define DEFAULT_UART_BUS_RX             (&pin_GPIO20)
#define DEFAULT_UART_BUS_TX             (&pin_GPIO21)

// For entering safe mode
#define CIRCUITPY_BOOT_BUTTON           (&pin_GPIO9)
