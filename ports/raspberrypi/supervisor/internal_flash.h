/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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
#ifndef MICROPY_INCLUDED_RASPBERRYPI_INTERNAL_FLASH_H
#define MICROPY_INCLUDED_RASPBERRYPI_INTERNAL_FLASH_H

#include <stdbool.h>

#include "mpconfigport.h"

// #define INTERNAL_FLASH_PART1_NUM_BLOCKS (CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE / FILESYSTEM_BLOCK_SIZE)

// #define INTERNAL_FLASH_SYSTICK_MASK    (0x1ff) // 512ms
// #define INTERNAL_FLASH_IDLE_TICK(tick) (((tick) & INTERNAL_FLASH_SYSTICK_MASK) == 2)

#endif  // MICROPY_INCLUDED_RASPBERRYPI_INTERNAL_FLASH_H
