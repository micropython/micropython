/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_MIMXRT10XX_INTERNAL_FLASH_H
#define MICROPY_INCLUDED_MIMXRT10XX_INTERNAL_FLASH_H

#include <stdbool.h>
#include <stdint.h>

#include "py/mpconfig.h"
#include "fsl_common.h"

#define INTERNAL_FLASH_SYSTICK_MASK     (0x1ff) // 512ms
#define INTERNAL_FLASH_IDLE_TICK(tick)  (((tick) & INTERNAL_FLASH_SYSTICK_MASK) == 2)

#define FLASH_PAGE_SIZE 256

#define ROM_INDEX_WRITEENABLE 3
#define ROM_INDEX_ERASESECTOR 5
#define ROM_INDEX_PAGEPROGRAM 9
#define ROM_INDEX_READSTATUSREG 1

extern void flexspi_nor_init(void);
extern status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address);
extern status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t dstAddr, const uint32_t *src);
extern status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base);


#endif  // MICROPY_INCLUDED_MIMXRT10XX_INTERNAL_FLASH_H
