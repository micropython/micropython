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

#ifndef FLASH_DISKIO_H_
#define FLASH_DISKIO_H_

// ESP SDK API
#include "c_types.h"
#include "spi_flash.h"

#define FLASH_BLOCK_SIZE        (SPI_FLASH_SEC_SIZE)
#define FLASH_BLOCK_COUNT       (16)
#define FLASH_SECTOR_SIZE       (512)
#define FLASH_SECTOR_COUNT      (1024)
#define FLASH_SECTORS_PER_BLOCK (FLASH_BLOCK_SIZE / FLASH_SECTOR_SIZE)

// first and last available sectors
#define FLASH_FATFS_START       (96)
#define FLASH_FATFS_END         (124)

// flash disk operations
DRESULT flash_disk_init(void);
DRESULT flash_disk_status(void);
DRESULT flash_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT flash_disk_write(const BYTE *buff, DWORD sector, UINT count);
DRESULT flash_disk_erase(DWORD sector);
DRESULT flash_disk_flush(void);
UINT    flash_disk_get_id(void);

#endif /* FLASH_DISKIO_H_ */
