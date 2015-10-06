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

#ifndef SFLASH_DISKIO_H_
#define SFLASH_DISKIO_H_

// ESP SDK API
#include "c_types.h"
#include "spi_flash.h"

#include "mpconfigport.h"

// sectors according to flash memory
#ifdef FLASH_AUTOSIZE
    #define _SECTOR_COUNT (sflash_disk_get_sector_count())
#elif defined(FLASH_512K)
    #define _SECTOR_COUNT 0x80
#elif defined(FLASH_1M)
    #define _SECTOR_COUNT 0x100
#elif defined(FLASH_2M)
    #define _SECTOR_COUNT 0x200
#elif defined(FLASH_4M)
    #define _SECTOR_COUNT 0x400
#elif defined(FLASH_8M)
    #define _SECTOR_COUNT 0x800
#elif defined(FLASH_16M)
    #define _SECTOR_COUNT 0x1000
// default value 512K
#else
    #define _SECTOR_COUNT 0x80
#endif // FLASH_AUTOSIZE

// TODO: correct block size/count
#define FLASH_START_ADDR   0x6c000
#define FLASH_SECTOR_SIZE  SPI_FLASH_SEC_SIZE
#define FLASH_SECTOR_COUNT (_SECTOR_COUNT * FLASH_BLOCK_COUNT)
#define FLASH_TOTAL_SIZE   (_SECTOR_COUNT * FLASH_SECTOR_SIZE)
// #define FLASH_WRITE_SIZE  4
// #define FLASH_READ_SIZE   4

#define FLASH_BLOCK_SIZE    65536
#define FLASH_BLOCK_COUNT   MICROPY_PORT_SFLASH_BLOCK_COUNT
// #define SFLASH_SECTORS_PER_BLOCK   (SFLASH_BLOCK_SIZE / SFLASH_SECTOR_SIZE)
// #define FLASH_PARAM_SECTOR_COUNT   4
// #define FLASH_PARAM_SECTOR_START   (FLASH_SECTOR_COUNT - FLASH_PARAM_SECTOR_COUNT)

// flash disk operations
DRESULT sflash_disk_init(void);
DRESULT sflash_disk_status(void);
DRESULT sflash_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT sflash_disk_write(const BYTE *buff, DWORD sector, UINT count);
DRESULT sflash_disk_flush(void);
uint32_t sflash_disk_get_id(void);
uint16_t sflash_disk_get_sector_count(void);

#endif /* SFLASH_DISKIO_H_ */