// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "nrfx/hal/nrf_gpio.h"

#define MICROPY_HW_BOARD_NAME       "iLabs Challenger 840"
#define MICROPY_HW_MCU_NAME         "nRF52840"

#define MICROPY_HW_LED_STATUS       (&pin_P0_12)

#if SPI_FLASH_FILESYSTEM
#define SPI_FLASH_MOSI_PIN          (&pin_P0_16)
#define SPI_FLASH_MISO_PIN          (&pin_P0_11)
#define SPI_FLASH_SCK_PIN           (&pin_P0_14)
#define SPI_FLASH_CS_PIN            (&pin_P0_08)
#define SPI_FLASH_MAX_BAUDRATE      20000000
#endif

#define CIRCUITPY_AUTORELOAD_DELAY_MS 500

#define CIRCUITPY_INTERNAL_NVM_SIZE (4096)

#define BOARD_FLASH_SIZE            (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#define BOARD_HAS_CRYSTAL           1

#define DEFAULT_I2C_BUS_SCL         (&pin_P1_12)
#define DEFAULT_I2C_BUS_SDA         (&pin_P1_11)

#define DEFAULT_SPI_BUS_SCK         (&pin_P0_13)
#define DEFAULT_SPI_BUS_MOSI        (&pin_P0_15)
#define DEFAULT_SPI_BUS_MISO        (&pin_P0_17)

#define DEFAULT_UART_BUS_RX         (&pin_P0_21)
#define DEFAULT_UART_BUS_TX         (&pin_P0_23)
