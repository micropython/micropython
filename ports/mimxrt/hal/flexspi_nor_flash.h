/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_MIMXRT_HAL_FLEXSPI_NOR_FLASH_H
#define MICROPY_INCLUDED_MIMXRT_HAL_FLEXSPI_NOR_FLASH_H

#include "fsl_flexspi.h"
#include "mpconfigboard.h"
#include BOARD_FLASH_CONFIG_HEADER_H

// Defined in boards flash_config.c
extern flexspi_nor_config_t qspiflash_config;

status_t flexspi_nor_get_vendor_id(FLEXSPI_Type *base, uint8_t *vendorId);
status_t flexspi_nor_init(void);
void flexspi_nor_update_lut(void);
status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address);
status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t address, const uint32_t *src, uint32_t size);

static inline uint32_t flexspi_get_frequency(void) {
    uint32_t div;
    uint32_t fre;

    /* Clock divider:
       000 divided by 1
       001 divided by 2
       010 divided by 3
       011 divided by 4
       100 divided by 5
       101 divided by 6
       110 divided by 7
       111 divided by 8
     */
    div = CLOCK_GetDiv(kCLOCK_FlexspiDiv);
    /* Get frequency */
    fre = CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk) / (div + 0x01U);

    return fre;
}

#endif // MICROPY_INCLUDED_MIMXRT_HAL_FLEXSPI_NOR_FLASH_H
