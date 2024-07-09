// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Bill Sideris, independently providing these changes.
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "DFRobot FireBeetle 2 ESP32-S3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO2, .sda = &pin_GPIO1}}

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO17)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO15)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO16)

#define MICROPY_HW_LED_STATUS (&pin_GPIO21)

#define DEFAULT_UART_BUS_RX (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX (&pin_GPIO43)

#define CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP (1)
