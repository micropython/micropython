// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
// SPDX-FileCopyrightText: Copyright (c) 2021 skieast/Bruce Segal
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME       "Deneyap Kart G"
#define MICROPY_HW_MCU_NAME         "ESP32-C3FN4"

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO2, .sda = &pin_GPIO8}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO4, .mosi = &pin_GPIO6, .miso = &pin_GPIO5}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO21, .rx = &pin_GPIO20}}

// For entering safe mode
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO9)
