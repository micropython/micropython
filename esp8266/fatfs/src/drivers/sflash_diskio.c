/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Alex March
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

#include <stdbool.h>

// ESP SDK API
#include "c_types.h"
#include "spi_flash.h"

#include "ffconf.h"
#include "diskio.h"
#include "sflash_diskio.h"

/******************************************************************************
 DECLARE PRIVATE DATA
 *****************************************************************************/
static bool sflash_init_done      = false;
static bool sflash_cache_is_dirty = false;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 *****************************************************************************/
// TODO: need to lock/unlock context for safe access
static bool sflash_access(void) {
    return true;
}

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 *****************************************************************************/
DRESULT sflash_disk_status(void) {
    if (!sflash_init_done) {
        return STA_NOINIT;
    }
    return RES_OK;
}

DRESULT sflash_disk_init(void) {
    if (!sflash_init_done) {
        sflash_init_done      = true;
        sflash_cache_is_dirty = false;

        // TODO: check if the file system already exists by checking first 2
        //       blocks, otherwise erase all sectors
    }
    return RES_OK;
}

// we can also set up a call back function for user read using
// spi_flash_set_read_func() in spi_flash.h but also need a SpiFlashChip obj
DRESULT sflash_disk_read(BYTE *buff, DWORD sector, UINT count) {

    DWORD addr = FLASH_START_ADDR + sector * FLASH_SECTOR_SIZE;
    UINT size  = count * FLASH_SECTOR_SIZE;

    if (!sflash_init_done) {
        return STA_NOINIT;
    }

    if ((sector + count > FLASH_SECTOR_COUNT) || (count == 0)) {
        return RES_PARERR;
    }

    // not catching the timeout
    if (SPI_FLASH_RESULT_OK == spi_flash_read(addr, (uint32_t *) buff, size)) {
        return RES_OK;
    } else {
        return RES_ERROR;
    }
}

#if _USE_WRITE
DRESULT sflash_disk_write(const BYTE *buff, DWORD sector, UINT count) {

    DWORD addr = FLASH_START_ADDR + sector * FLASH_SECTOR_SIZE;
    UINT size  = count * FLASH_SECTOR_SIZE;

    if (!sflash_init_done) {
        return STA_NOINIT;
    }

    if ((sector + count > FLASH_SECTOR_COUNT) || (count == 0)) {
        sflash_disk_flush();
        return RES_PARERR;
    }

    // not catching the timeout
    if (SPI_FLASH_RESULT_OK == spi_flash_erase_sector(addr / FLASH_SECTOR_SIZE)) {
        if (SPI_FLASH_RESULT_OK == spi_flash_write(addr, (uint32_t *) buff, size)) {
            sflash_cache_is_dirty = true;
            return RES_OK;
        }
    }
    return RES_ERROR;
}
#endif

DRESULT sflash_disk_erase(DWORD sector) {

    if (!sflash_init_done) {
        return STA_NOINIT;
    }

    if (sector > FLASH_SECTOR_COUNT || sector == 0) {
        return RES_PARERR;
    }

    if (SPI_FLASH_RESULT_OK == spi_flash_erase_sector(sector)) {
        return RES_OK;
    }
    return RES_ERROR;
}

DRESULT sflash_disk_flush(void) {
    // write back the cache if it's dirty
    if (sflash_cache_is_dirty) {
        if (!sflash_access()) {
            return RES_ERROR;
        }
        sflash_cache_is_dirty = false;
    }
    return RES_OK;
}

uint32_t sflash_disk_get_id(void) {
    return spi_flash_get_id();
}

// TODO: auto count different flash sizes
uint16_t sflash_disk_get_sector_count(void) {
    static uint32_t flash_total_size = 0;
    if (0 == flash_total_size) {
        flash_total_size = 4 * 1024 * 1024; // 4Mbit
        flash_total_size /= 8;              // 512Kb
    }
    return flash_total_size / FLASH_SECTOR_SIZE;
}