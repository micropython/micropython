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
#include "spi_flash.h"
#include "c_types.h"

#include "ffconf.h"
#include "diskio.h"
#include "sflash_diskio.h"

static bool sflash_init_done      = false;
static bool sflash_cache_is_dirty = false;

static volatile const uint8_t
sflash_init_data[128] ICACHE_STORE_ATTR ICACHE_RODATA_ATTR =
{
    0x05, 0x00, 0x04, 0x02, 0x05, 0x05, 0x05, 0x02,
    0x05, 0x00, 0x04, 0x05, 0x05, 0x04, 0x05, 0x05,
    0x04, 0xFE, 0xFD, 0xFF, 0xF0, 0xF0, 0xF0, 0xE0,
    0xE0, 0xE0, 0xE1, 0x0A, 0xFF, 0xFF, 0xF8, 0x00,
    0xF8, 0xF8, 0x52, 0x4E, 0x4A, 0x44, 0x40, 0x38,
    0x00, 0x00, 0x01, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xE1, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x93, 0x43, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// init FS using ^ data
// DRESULF sflash_disk_init_default(void)
DRESULT sflash_disk_init (void) {

    if (!sflash_init_done) {
        sflash_init_done      = true;
        sflash_cache_is_dirty = false;

        // init
    }
    return RES_OK;
}

// need to lock/unlock context for safe access
static bool sflash_access(void) {
    return true;
}

DRESULT sflash_disk_status(void) {
    if (!sflash_init_done) {
        return STA_NOINIT;
    }
    return RES_OK;
}

DRESULT sflash_disk_read(BYTE *buff, DWORD sector, UINT count) {

    if (!sflash_init_done) {
        return STA_NOINIT;
    }

    if ((sector + count > SFLASH_SECTOR_COUNT) || (count == 0)) {
        return RES_PARERR;
    }

    // spi_flash_read() can be called from the spi_flash.h
    // SpiFlashOpResult spi_flash_read(uint32 src_addr, uint32 *des_addr, uint32 size);

    return RES_OK;
}

DRESULT sflash_disk_write(const BYTE *buff, DWORD sector, UINT count) {

    if (!sflash_init_done) {
        return STA_NOINIT;
    }

    if ((sector + count > SFLASH_SECTOR_COUNT) || (count == 0)) {
        sflash_disk_flush();
        return RES_PARERR;
    }

    // spi_flash_write can be called from the spi_flash.h
    // SpiFlashOpResult spi_flash_write(uint32 des_addr, uint32 *src_addr, uint32 size);

    sflash_cache_is_dirty = true;
    return RES_OK;
}

DRESULT sflash_disk_flush (void) {
    // write back the cache if it's dirty
    if (sflash_cache_is_dirty) {
        // check access
        // if (!sflash_access()) {
        //     return RES_ERROR;
        // }
    }
    sflash_cache_is_dirty = false;
    return RES_OK;
}

// AUTOSIZE
uint16_t sflash_get_sec_count(void) {
    // return (sflash_get_size_byte() / SPI_FLASH_SEC_SIZE)
    return 0;
}