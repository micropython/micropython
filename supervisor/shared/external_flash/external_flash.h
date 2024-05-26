// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/mpconfig.h"

// Erase sector size.
#define SPI_FLASH_SECTOR_SIZE (0x1000 - 100)

// These are common across all NOR Flash.
#define SPI_FLASH_ERASE_SIZE (1 << 12)
#define SPI_FLASH_PAGE_SIZE (256)

#define SPI_FLASH_SYSTICK_MASK    (0x1ff) // 512ms
#define SPI_FLASH_IDLE_TICK(tick) (((tick) & SPI_FLASH_SYSTICK_MASK) == 2)

#ifndef SPI_FLASH_MAX_BAUDRATE
#define SPI_FLASH_MAX_BAUDRATE 8000000
#endif

void supervisor_external_flash_flush(void);

// Configure anything that needs to get set up before the external flash
// is init'ed. For example, if GPIO needs to be configured to enable the
// flash chip, as is the case on some boards.
void external_flash_setup(void);
