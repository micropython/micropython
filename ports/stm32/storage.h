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
#ifndef MICROPY_INCLUDED_STM32_STORAGE_H
#define MICROPY_INCLUDED_STM32_STORAGE_H

#include "drivers/memory/spiflash.h"

#define FLASH_BLOCK_SIZE (512)
#define FLASH_PART1_START_BLOCK (0x100)

// Try to match Python-level VFS block protocol where possible for these constants
enum {
    BDEV_IOCTL_INIT = 1,
    BDEV_IOCTL_SYNC = 3,
    BDEV_IOCTL_NUM_BLOCKS = 4,
    BDEV_IOCTL_BLOCK_ERASE = 6,
    BDEV_IOCTL_IRQ_HANDLER = 7,
};

void storage_init(void);
uint32_t storage_get_block_size(void);
uint32_t storage_get_block_count(void);
void storage_flush(void);
bool storage_read_block(uint8_t *dest, uint32_t block);
bool storage_write_block(const uint8_t *src, uint32_t block);

// these return 0 on success, negative errno on error
int storage_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks);
int storage_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks);
int storage_readblocks_ext(uint8_t *dest, uint32_t block, uint32_t offset, uint32_t len);

int32_t flash_bdev_ioctl(uint32_t op, uint32_t arg);
bool flash_bdev_readblock(uint8_t *dest, uint32_t block);
bool flash_bdev_writeblock(const uint8_t *src, uint32_t block);
int flash_bdev_readblocks_ext(uint8_t *dest, uint32_t block, uint32_t offset, uint32_t len);
int flash_bdev_writeblocks_ext(const uint8_t *src, uint32_t block, uint32_t offset, uint32_t len);

typedef struct _spi_bdev_t {
    mp_spiflash_t spiflash;
    uint32_t flash_tick_counter_last_write;
} spi_bdev_t;

int32_t spi_bdev_ioctl(spi_bdev_t *bdev, uint32_t op, uint32_t arg);
int spi_bdev_readblocks(spi_bdev_t *bdev, uint8_t *dest, uint32_t block_num, uint32_t num_blocks);
int spi_bdev_writeblocks(spi_bdev_t *bdev, const uint8_t *src, uint32_t block_num, uint32_t num_blocks);

// These raw functions bypass the cache and go directly to SPI flash
int spi_bdev_readblocks_raw(spi_bdev_t *bdev, uint8_t *dest, uint32_t block_num, uint32_t block_offset, uint32_t num_bytes);
int spi_bdev_writeblocks_raw(spi_bdev_t *bdev, const uint8_t *src, uint32_t block_num, uint32_t block_offset, uint32_t num_bytes);

extern const struct _mp_obj_type_t pyb_flash_type;
extern const struct _pyb_flash_obj_t pyb_flash_obj;

struct _fs_user_mount_t;
void pyb_flash_init_vfs(struct _fs_user_mount_t *vfs);

#endif // MICROPY_INCLUDED_STM32_STORAGE_H
