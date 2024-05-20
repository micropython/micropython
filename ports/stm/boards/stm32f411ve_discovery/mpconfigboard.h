// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "STM32F411E_DISCO"
#define MICROPY_HW_MCU_NAME         "STM32F411xE"

#define FLASH_SIZE                  (0x80000) // 512K
#define FLASH_PAGE_SIZE             (0x4000)  // 16K

#define HSE_VALUE ((uint32_t)8000000)
#define BOARD_HSE_SOURCE (RCC_HSE_BYPASS) // ST boards use the STLink clock signal
#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

// The schematic has a 32k crystal that isn't fitted. Uncommented the line below if you add it.
// #define BOARD_HAS_LOW_SPEED_CRYSTAL (1)
// #define LSE_VALUE  ((uint32_t)32000U)

#define MICROPY_FATFS_EXFAT 0
