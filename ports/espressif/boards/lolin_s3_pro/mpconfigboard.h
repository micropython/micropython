// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "LOLIN S3 PRO 16MB Flash 8MB PSRAM"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO38)

// #define MICROPY_HW_LED_STATUS (&pin_GPIO14)

#define DEFAULT_BUTTON (&pin_GPIO0)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO10)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO9)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO12)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO13)

#define DEFAULT_TF_CS (&pin_GPIO46)
#define DEFAULT_TS_CS (&pin_GPIO45)
#define DEFAULT_TFT_CS (&pin_GPIO48)
#define DEFAULT_TFT_DC (&pin_GPIO47)
#define DEFAULT_TFT_RST (&pin_GPIO21)
#define DEFAULT_TFT_LED (&pin_GPIO14)


#define DEFAULT_TX0 (&pin_GPIO43)
#define DEFAULT_RX0 (&pin_GPIO44)
