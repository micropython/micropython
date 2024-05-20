// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Espruino Wifi"
#define MICROPY_HW_MCU_NAME         "STM32F411xE"

#define FLASH_SIZE                  (0x80000) // 512K
#define FLASH_PAGE_SIZE             (0x4000)  // 16K

#define HSE_VALUE ((uint32_t)8000000)
#define LSE_VALUE ((uint32_t)32768)
#define BOARD_HAS_LOW_SPEED_CRYSTAL (1)
#define BOARD_OVERWRITE_SWD (1)
