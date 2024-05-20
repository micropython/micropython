// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "MEOWBIT"
#define MICROPY_HW_MCU_NAME         "STM32F401xE"

#define FLASH_SIZE                  (0x80000)
#define FLASH_PAGE_SIZE             (0x4000)

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000)

#define HSE_VALUE ((uint32_t)12000000U)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

#define BOARD_NO_VBUS_SENSE (1)
#define BOARD_VTOR_DEFER (1) // Leave VTOR relocation to bootloader
#define BOARD_USE_INTERNAL_SPI

// On-board flash
#define SPI_FLASH_MOSI_PIN          (&pin_PB15)
#define SPI_FLASH_MISO_PIN          (&pin_PB14)
#define SPI_FLASH_SCK_PIN           (&pin_PB13)
#define SPI_FLASH_CS_PIN            (&pin_PB01)
