/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_SUPERVISOR_SHARED_EXTERNAL_FLASH_EXTERNAL_FLASH_H
#define MICROPY_INCLUDED_SUPERVISOR_SHARED_EXTERNAL_FLASH_EXTERNAL_FLASH_H

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

#endif  // MICROPY_INCLUDED_SUPERVISOR_SHARED_EXTERNAL_FLASH_EXTERNAL_FLASH_H
