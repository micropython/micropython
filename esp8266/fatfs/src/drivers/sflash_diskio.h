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
//#include "spi_flash.h"
#include "mpconfigport.h"

// sector size according to flash memory
#ifdef SFLASH_AUTOSIZE
    #define _SECTOR_SIZE (sflash_get_sec_count())
#elif defined(SFLASH_512K)
    #define _SECTOR_SIZE 0x80
#elif defined(SFLASH_1M)
    #define _SECTOR_SIZE 0x100
#elif defined(SFLASH_2M)
    #define _SECTOR_SIZE 0x200
#elif defined(SFLASH_4M)
    #define _SECTOR_SIZE 0x400
#elif defined(SFLASH_8M)
    #define _SECTOR_SIZE 0x800
#elif defined(SFLASH_16M)
    #define _SECTOR_SIZE 0x1000
// default value 512K
#else
    #define _SECTOR_SIZE 0x80
#endif // SFLASH_AUTOSIZE

// TODO: flash size definitions here, correct block size/count
//       possibly the definition of BLOCK/SECTOR is vice versa in ESP SDK
//       INTERNAL FLASH sizes or total sizes
#define SFLASH_BLOCK_SIZE    4096 // as defined in spi_flash.h
#define SFLASH_BLOCK_COUNT   MICROPY_PORT_SFLASH_BLOCK_COUNT // or block_size / sector size
#define SFLASH_SECTOR_SIZE   _SECTOR_SIZE
#define SFLASH_SECTOR_COUNT  ((SFLASH_BLOCK_SIZE * SFLASH_BLOCK_COUNT)) / SFLASH_SECTOR_SIZE

#define SFLASH_SECTORS_PER_BLOCK (SFLASH_BLOCK_SIZE / SFLASH_SECTOR_SIZE)

// last 4 blocks used by ESP libs
#define SFLASH_PARAM_SEC_COUNT    4
#define SFLASH_PARAM_SEC_START    (SFLASH_SECTOR_COUNT - SFLASH_PARAM_SEC_NUM)

#define SFLASH_START_ADDR  0x40200000
#define SFLASH_TOTAL_SIZE  ((SFLASH_PARAM_SEC_START) * SFLASH_SECTOR_SIZE))
#define SFLASH_WRITE_SIZE  4
#define SFLASH_READ_SIZE   4

// flash disk operations
DRESULT sflash_disk_init(void);
DRESULT sflash_disk_status(void);
DRESULT sflash_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT sflash_disk_write(const BYTE *buff, DWORD sector, UINT count);
DRESULT sflash_disk_flush(void);

#endif /* SFLASH_DISKIO_H_ */