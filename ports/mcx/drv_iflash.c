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

#include "board.h"

#include "fsl_cache_lpcac.h"

#include "drv_iflash.h"

int drv_iflash_init(drv_iflash_t *iflash) {
    status_t ret = FLASH_Init(&iflash->flash_cfg);
    if (ret != kStatus_Success) {
        return -1;
    }

    return 0;
}

int drv_iflash_attr_get(drv_iflash_t *iflash, drv_iflash_attr_t *attr) {
    status_t ret;

    ret = FLASH_GetProperty(&iflash->flash_cfg, kFLASH_PropertyPflashSectorSize, &attr->sector_size);
    if (ret != kStatus_Success) {
        return -1;
    }

    ret = FLASH_GetProperty(&iflash->flash_cfg, kFLASH_PropertyPflashPageSize, &attr->page_size);
    if (ret != kStatus_Success) {
        return -1;
    }

    return 0;
}

int drv_iflash_read(drv_iflash_t *iflash, uint32_t addr, uint8_t *data, uint32_t len) {
    memcpy(data, (uint8_t *)addr, len);
    return 0;
}

int drv_iflash_erase_sector(drv_iflash_t *iflash, uint32_t sector_addr) {
    uint32_t sector_size = 0U;
    status_t ret = FLASH_GetProperty(&iflash->flash_cfg, kFLASH_PropertyPflashSectorSize, &sector_size);
    if (ret != kStatus_Success) {
        return -1;
    }

    ret = FLASH_Erase(&iflash->flash_cfg, sector_addr, sector_size, kFLASH_ApiEraseKey);
    if (ret != kStatus_Success) {
        return -1;
    }

    L1CACHE_InvalidateCodeCache();

    return 0;
}

int drv_iflash_program_page(drv_iflash_t *iflash, uint32_t page_addr, uint8_t *data) {
    uint32_t page_size = 0U;
    status_t ret = FLASH_GetProperty(&iflash->flash_cfg, kFLASH_PropertyPflashPageSize, &page_size);
    if (ret != kStatus_Success) {
        return -1;
    }

    ret = FLASH_Program(&iflash->flash_cfg, page_addr, data, page_size);
    if (ret != kStatus_Success) {
        return -1;
    }

    L1CACHE_InvalidateCodeCache();

    return 0;
}
