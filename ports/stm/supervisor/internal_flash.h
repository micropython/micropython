/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
 * Copyright (c) 2020 Mark Olsson <mark@markolsson.se>
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
#ifndef MICROPY_INCLUDED_STM32_INTERNAL_FLASH_H
#define MICROPY_INCLUDED_STM32_INTERNAL_FLASH_H

#include <stdbool.h>
#include <stdint.h>

#include "py/mpconfig.h"

#ifdef STM32F401xE
#define STM32_FLASH_SIZE 0x80000 // 512KiB
#define INTERNAL_FLASH_FILESYSTEM_SIZE 0xC000 // 48KiB
#define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x08004000
#endif

#ifdef STM32F411xE
#define STM32_FLASH_SIZE 0x80000 // 512KiB
#ifndef INTERNAL_FLASH_FILESYSTEM_SIZE
  #define INTERNAL_FLASH_FILESYSTEM_SIZE 0xC000 // 48KiB
#endif
#ifndef INTERNAL_FLASH_FILESYSTEM_START_ADDR
  #define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x08004000
#endif
#endif

#ifdef STM32F412Zx
#define STM32_FLASH_SIZE 0x100000 // 1MB
#define INTERNAL_FLASH_FILESYSTEM_SIZE 0xC000 // 48KiB
#define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x08004000
#endif

#ifdef STM32F405xx
#define STM32_FLASH_SIZE 0x100000 // 1MB
#define INTERNAL_FLASH_FILESYSTEM_SIZE 0xC000 // 48KiB
#define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x08004000
#endif

#ifdef STM32F407xx
#define STM32_FLASH_SIZE 0x100000 // 1MB
#define INTERNAL_FLASH_FILESYSTEM_SIZE 0xC000 // 48KiB
#define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x08004000
#endif

/* Note this applies to STM32F769xG only, STM32F746xE has 512KB */
#ifdef STM32F746xx
#define STM32_FLASH_SIZE 0x100000 // 1MB
#define INTERNAL_FLASH_FILESYSTEM_SIZE 0x18000 // 96KiB
#define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x08008000
#endif

#ifdef STM32F767xx
#define STM32_FLASH_SIZE 0x200000 // 2MB
#define INTERNAL_FLASH_FILESYSTEM_SIZE 0x18000 // 96KiB
#define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x08008000
#endif

#ifdef STM32H743xx
#define STM32_FLASH_SIZE 0x200000 // 2MB
#define INTERNAL_FLASH_FILESYSTEM_SIZE 0x60000 // 384KiB
#define INTERNAL_FLASH_FILESYSTEM_START_ADDR 0x08020000
#endif

#define INTERNAL_FLASH_FILESYSTEM_NUM_BLOCKS (INTERNAL_FLASH_FILESYSTEM_SIZE / FILESYSTEM_BLOCK_SIZE)

#define STM32_FLASH_OFFSET 0x8000000 // All STM32 chips map to this flash location

#define INTERNAL_FLASH_SYSTICK_MASK     (0x1ff) // 512ms
#define INTERNAL_FLASH_IDLE_TICK(tick)  (((tick) & INTERNAL_FLASH_SYSTICK_MASK) == 2)

#endif  // MICROPY_INCLUDED_STM32_INTERNAL_FLASH_H
