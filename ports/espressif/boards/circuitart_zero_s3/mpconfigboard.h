// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "CircuitART Zero S3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO47)
//#define CIRCUITPY_STATUS_LED_POWER (&pin_GPIO39)

#define MICROPY_HW_LED_STATUS (&pin_GPIO46)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO34)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO33)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO36)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO35)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO37)

#define DEFAULT_UART_BUS_RX (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX (&pin_GPIO43)

#define DOUBLE_TAP_PIN (&pin_GPIO45)

#define DEFAULT_TF_CS (&pin_GPIO42)
#define DEFAULT_TFT_CS (&pin_GPIO39)
#define DEFAULT_TFT_DC (&pin_GPIO5)
#define DEFAULT_TFT_RST (&pin_GPIO40)
