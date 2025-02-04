/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#ifndef MP_PORT_MCX_DRV_IFLASH_H
#define MP_PORT_MCX_DRV_IFLASH_H

#include <stdint.h>

#include "fsl_flash.h"

typedef struct drv_iflash_attr_type {
    uint32_t sector_size;
    uint32_t page_size;
} drv_iflash_attr_t;

typedef struct {
    flash_config_t flash_cfg;
} drv_iflash_t;

int drv_iflash_init(drv_iflash_t *iflash);
int drv_iflash_attr_get(drv_iflash_t *iflash, drv_iflash_attr_t *attr);
int drv_iflash_read(drv_iflash_t *iflash, uint32_t addr, uint8_t *data, uint32_t len);
int drv_iflash_erase_sector(drv_iflash_t *iflash, uint32_t sector_addr);
int drv_iflash_program_page(drv_iflash_t *iflash, uint32_t page_addr, uint8_t *data);

#endif
