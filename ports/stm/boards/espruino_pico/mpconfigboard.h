// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Espruino Pico"
#define MICROPY_HW_MCU_NAME         "STM32F401xD"

#define FLASH_SIZE                  (0x60000)
#define FLASH_PAGE_SIZE             (0x4000)

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000)

#define HSE_VALUE ((uint32_t)8000000)

// Replace LSE lines and recompile if you add a low power crystal
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)
// #define BOARD_HAS_LOW_SPEED_CRYSTAL (1)
// #define LSE_VALUE ((uint32_t)32768)

#define MICROPY_FATFS_EXFAT 0
