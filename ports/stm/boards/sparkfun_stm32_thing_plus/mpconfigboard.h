// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "SparkFun Thing Plus - STM32"
#define MICROPY_HW_MCU_NAME         "STM32F405RG"

#define FLASH_SIZE                  (0x100000)
#define FLASH_PAGE_SIZE             (0x4000)

#define HSE_VALUE ((uint32_t)12000000U)
#define LSE_VALUE ((uint32_t)32768)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (1)

// On-board flash
#define SPI_FLASH_MOSI_PIN          (&pin_PB05)
#define SPI_FLASH_MISO_PIN          (&pin_PB04)
#define SPI_FLASH_SCK_PIN           (&pin_PB03)
#define SPI_FLASH_CS_PIN            (&pin_PA15)

// Bootloader only
#ifdef UF2_BOOTLOADER_ENABLED
    #define BOARD_VTOR_DEFER (1) // Leave VTOR relocation to bootloader
#endif

#define DEFAULT_I2C_BUS_SCL (&pin_PB06)
#define DEFAULT_I2C_BUS_SDA (&pin_PB07)

#define DEFAULT_SPI_BUS_SCK (&pin_PB13)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB15)
#define DEFAULT_SPI_BUS_MISO (&pin_PB14)

#define DEFAULT_UART_BUS_RX (&pin_PB11)
#define DEFAULT_UART_BUS_TX (&pin_PB10)
