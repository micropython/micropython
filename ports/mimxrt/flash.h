/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Philipp Ebensberger
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

#ifndef MICROPY_INCLUDED_MIMXRT_FLASH_H
#define MICROPY_INCLUDED_MIMXRT_FLASH_H

#include BOARD_FLASH_OPS_HEADER_H

#define SECTOR_SIZE_BYTES (qspiflash_config.sectorSize)
#define PAGE_SIZE_BYTES (qspiflash_config.pageSize)
#define BLOCK_SIZE_BYTES (qspiflash_config.blockSize)

#define SECTOR_SIZE_BYTES (qspiflash_config.sectorSize)
#define PAGE_SIZE_BYTES (qspiflash_config.pageSize)

#ifndef MICROPY_HW_FLASH_STORAGE_BYTES
#define MICROPY_HW_FLASH_STORAGE_BYTES (((uint32_t)&__vfs_end) - ((uint32_t)&__vfs_start))
#endif

#ifndef MICROPY_HW_FLASH_STORAGE_BASE
#define MICROPY_HW_FLASH_STORAGE_BASE (((uint32_t)&__vfs_start) - ((uint32_t)&__flash_start))
#endif

// Linker symbols
extern uint8_t __vfs_start;
extern uint8_t __vfs_end;
extern uint8_t __flash_start;

void flash_init(void);
status_t flash_erase_sector(uint32_t erase_addr);
status_t flash_erase_block(uint32_t erase_addr);
void flash_read_block(uint32_t src_addr, uint8_t *dest, uint32_t length);
status_t flash_write_block(uint32_t dest_addr, const uint8_t *src, uint32_t length);

#endif // MICROPY_INCLUDED_MIMXRT_FLASH_H
