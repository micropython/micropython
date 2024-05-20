// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once
#define MICROPY_HW_BOARD_NAME       "THUNDERPACK_v11"
#define MICROPY_HW_MCU_NAME         "STM32F411CE"

// Non-volatile memory config
#define CIRCUITPY_INTERNAL_NVM_SIZE        (0x4000)
#define CIRCUITPY_INTERNAL_NVM_START_ADDR  (0x08010000)
#define CIRCUITPY_INTERNAL_NVM_SECTOR      FLASH_SECTOR_4

// Putting the entire flash sector in the NVM byte array buffer
// would take up too much RAM. This limits how much of the sector we use.
#define NVM_BYTEARRAY_BUFFER_SIZE   512

// Flash config
#define FLASH_SIZE                  (0x80000)
#define FLASH_PAGE_SIZE             (0x4000)
#define BOARD_FLASH_SIZE            (FLASH_SIZE - CIRCUITPY_INTERNAL_NVM_SIZE - 0x2000 - 0xC000)

#define HSE_VALUE                   ((uint32_t)24000000U)
#define BOARD_OVERWRITE_SWD         (1)
#define BOARD_NO_VBUS_SENSE         (1)

#define BOARD_HAS_LOW_SPEED_CRYSTAL (0)

// Status LEDs
#define MICROPY_HW_LED_STATUS       (&pin_PA02)

#define DEFAULT_I2C_BUS_SCL         (&pin_PB06)
#define DEFAULT_I2C_BUS_SDA         (&pin_PB07)

#define MICROPY_FATFS_EXFAT 0
