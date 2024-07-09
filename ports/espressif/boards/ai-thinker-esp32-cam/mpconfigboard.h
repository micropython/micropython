// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Chris Drake, independently providing these changes.
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "Ai Thinker ESP32-CAM"
#define MICROPY_HW_MCU_NAME         "ESP32"

#define MICROPY_HW_LED_STATUS       (&pin_GPIO33)
#define MICROPY_HW_LED_STATUS_INVERTED (1)


#define CIRCUITPY_BOARD_I2C         (2)
/* OV2640 camera I2C pins are GPIO27 and _GPIO26.  GPIO12 and GPIO13 are unused unless SD card inserted:- */
#define CIRCUITPY_BOARD_I2C_PIN     { {.scl = &pin_GPIO27, .sda = &pin_GPIO26}, \
                                      {.scl = &pin_GPIO12, .sda = &pin_GPIO13} }

// SD_CARD
#define DEFAULT_SPI_BUS_SCK (&pin_GPIO14)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO15)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO2)

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO3)

#define CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP (1)
