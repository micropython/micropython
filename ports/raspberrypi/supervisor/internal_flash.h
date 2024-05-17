// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#ifndef MICROPY_INCLUDED_RASPBERRYPI_INTERNAL_FLASH_H
#define MICROPY_INCLUDED_RASPBERRYPI_INTERNAL_FLASH_H

#include <stdbool.h>

#include "mpconfigport.h"

// #define INTERNAL_FLASH_PART1_NUM_BLOCKS (CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE / FILESYSTEM_BLOCK_SIZE)

// #define INTERNAL_FLASH_SYSTICK_MASK    (0x1ff) // 512ms
// #define INTERNAL_FLASH_IDLE_TICK(tick) (((tick) & INTERNAL_FLASH_SYSTICK_MASK) == 2)

#endif  // MICROPY_INCLUDED_RASPBERRYPI_INTERNAL_FLASH_H
