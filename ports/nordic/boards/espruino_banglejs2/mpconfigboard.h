// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "Espruino Bangle.js 2"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_HW_LED_STATUS       (&pin_P0_19)

#if SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN &pin_P0_15
#define SPI_FLASH_MISO_PIN &pin_P0_13
#define SPI_FLASH_SCK_PIN &pin_P0_16
#define SPI_FLASH_CS_PIN &pin_P0_14
#endif

#define CIRCUITPY_BOOT_BUTTON (&pin_P0_17)

#define BOARD_HAS_32KHZ_XTAL (1)
