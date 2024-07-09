// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Wai Weng for Cytron Technologies
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Cytron Maker Feather AIoT S3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO46)
#define CIRCUITPY_STATUS_LED_POWER (&pin_GPIO11)

#define MICROPY_HW_LED_STATUS (&pin_GPIO2)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO41)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO42)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO17)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO8)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO18)

#define DEFAULT_UART_BUS_RX (&pin_GPIO16)
#define DEFAULT_UART_BUS_TX (&pin_GPIO15)

#define DOUBLE_TAP_PIN (&pin_GPIO1)
