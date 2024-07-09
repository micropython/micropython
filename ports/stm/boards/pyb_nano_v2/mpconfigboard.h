// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "PYB LR Nano V2"
#define MICROPY_HW_MCU_NAME         "STM32F411CE"

#define FLASH_SIZE                  (0x80000)
#define FLASH_PAGE_SIZE             (0x4000)

#define HSE_VALUE ((uint32_t)8000000)
#define LSE_VALUE ((uint32_t)32768)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (1)

// On-board flash
#define SPI_FLASH_MOSI_PIN          (&pin_PB15)
#define SPI_FLASH_MISO_PIN          (&pin_PB14)
#define SPI_FLASH_SCK_PIN           (&pin_PB13)
#define SPI_FLASH_CS_PIN            (&pin_PB12)

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x2000 - 0xC000)

#define MICROPY_FATFS_EXFAT 0
