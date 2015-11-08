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
#include <string.h>

// ESP SDK API
#include "c_types.h"

#include "lib/fatfs/ffconf.h"
#include "lib/fatfs/diskio.h"
#include "flash.h"

/******************************************************************************
 DECLARE PRIVATE DATA
 *****************************************************************************/
static BYTE flash_cache       [FLASH_BLOCK_SIZE];  // cache buffer
static UINT flash_cache_block = 0;                 // current io block
static bool flash_cache_dirty = false;
static bool flash_init_done   = false;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 *****************************************************************************/
static DRESULT flash_cache_read() {
    if (SPI_FLASH_RESULT_OK == spi_flash_read(
        (FLASH_FATFS_START + flash_cache_block) * FLASH_BLOCK_SIZE,
        (UINT *) flash_cache,
        FLASH_BLOCK_SIZE)) {

        return RES_OK;
    }
    return RES_ERROR;
}

static DRESULT flash_cache_write() {
    if (SPI_FLASH_RESULT_OK == spi_flash_erase_sector(
        FLASH_FATFS_START + flash_cache_block)) {

        if (SPI_FLASH_RESULT_OK == spi_flash_write(
            (FLASH_FATFS_START + flash_cache_block) * FLASH_BLOCK_SIZE,
            (UINT *) flash_cache,
            FLASH_BLOCK_SIZE)) {

            return RES_OK;
        }
    }
    return RES_ERROR;
}

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 *****************************************************************************/
DRESULT flash_disk_status(void) {
    if (!flash_init_done) {
        return STA_NOINIT;
    }
    return RES_OK;
}

DRESULT flash_disk_init(void) {
    if (!flash_init_done) {
        flash_init_done   = true;
        flash_cache_dirty = false;

        memset(&flash_cache, 0, sizeof(flash_cache));
        flash_cache_block = UINT32_MAX;

        // TODO: check if the file system already exists by checking first 2
        //       blocks to speed up booting
    }
    return RES_OK;
}

DRESULT flash_disk_read(BYTE *buff, DWORD sector, UINT count) {
    UINT current_sector = 0;
    UINT current_block  = 0;

    if (!flash_init_done) {
        return STA_NOINIT;
    }
    if (((sector + count) > FLASH_SECTOR_COUNT) || (count == 0)) {
        return RES_PARERR;
    }

    for (UINT i = 0; i < count; i++) {
        current_sector   = (sector + i) % FLASH_SECTORS_PER_BLOCK;
        current_block  = (sector + i) / FLASH_SECTORS_PER_BLOCK;

        // flush the prev block and read new one into cache
        if (flash_cache_block != current_block) {
            if (RES_OK != flash_disk_flush()){
                return RES_ERROR;
            }
            flash_cache_block = current_block;
            flash_cache_read();
        }

        // read from cache
        memcpy(
            buff,
            &flash_cache[current_sector * FLASH_SECTOR_SIZE],
            FLASH_SECTOR_SIZE);
        buff += FLASH_SECTOR_SIZE;
    }
    return RES_OK;
}

#if _USE_WRITE
DRESULT flash_disk_write(const BYTE *buff, DWORD sector, UINT count) {
    UINT current_sector = 0;
    UINT current_block  = 0;

    if (!flash_init_done) {
        return STA_NOINIT;
    }
    if (((sector + count) > FLASH_SECTOR_COUNT) || (count == 0)) {
        return RES_PARERR;
    }

    for (UINT i = 0; i < count; i++) {
        current_sector = (sector + i) % FLASH_SECTORS_PER_BLOCK;
        current_block  = (sector + i) / FLASH_SECTORS_PER_BLOCK;

        // flush the prev block and read new one into cache
        if (flash_cache_block != current_block) {
            if (RES_OK != flash_disk_flush()){
                return RES_ERROR;
            }
            flash_cache_block = current_block;
            flash_cache_read();
        }

        // write to cache
        memcpy(
            &flash_cache[current_sector * FLASH_SECTOR_SIZE],
            buff,
            FLASH_SECTOR_SIZE);
        buff += FLASH_SECTOR_SIZE;
        flash_cache_dirty = true;
    }
    return RES_OK;
}
#endif

DRESULT flash_disk_flush(void) {
    // write back the cache if it's dirty
    if (flash_cache_dirty) {
        if (RES_OK != flash_cache_write()) {
            return RES_ERROR;
        }
        flash_cache_dirty = false;
    }
    return RES_OK;
}

UINT flash_disk_get_id(void) {
    return spi_flash_get_id();
}
