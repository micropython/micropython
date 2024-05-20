// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "IMXRT1015-EVK"
#define MICROPY_HW_MCU_NAME "IMXRT1015DAF5A"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define BOARD_FLASH_SIZE (16 * 1024 * 1024)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO_AD_B0_10)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO_AD_B0_12)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO_AD_B0_13)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO_AD_B1_15)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO_AD_B1_14)

#define DEFAULT_UART_BUS_RX (&pin_GPIO_EMC_33)
#define DEFAULT_UART_BUS_TX (&pin_GPIO_EMC_32)

// #define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO_AD_B0_07)
// #define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO_AD_B0_06)

#define MICROPY_HW_LED_STATUS (&pin_GPIO_SD_B1_00)
#define MICROPY_HW_LED_STATUS_INVERTED (1)
