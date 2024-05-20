// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Fomu"
#define MICROPY_HW_MCU_NAME         "VexRiscv"

#define FLASH_SIZE                  (0x100000)
#define FLASH_PAGE_SIZE             (0x1000)
#define FLASH_PARTITION_OFFSET_BYTES (1024 * 1024)

#define BOARD_FLASH_SIZE            (FLASH_SIZE)
