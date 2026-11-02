// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 ThePiHut
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME       "ThePiHut Baguette S3"
#define MICROPY_HW_MCU_NAME         "ESP32-S3-MINI-1-N8"

// Status LED (GPIO8)
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO8)

// I2C (Qwiic connector)
#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO48, .sda = &pin_GPIO47}}

// SPI bus wired to the on-board SD card slot (exposed as board.SD_SPI)
#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO9, .mosi = &pin_GPIO10, .miso = &pin_GPIO11}}

// UART
#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO43, .rx = &pin_GPIO44}}

// For entering safe mode
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)
