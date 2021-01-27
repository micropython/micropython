/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#include <string.h>

#include "supervisor/flash.h"

/* Prototypes for Remote API */

int fw_fm_rawwrite(uint32_t offset, const void *buf, uint32_t size);
int fw_fm_rawverifywrite(uint32_t offset, const void *buf, uint32_t size);
int fw_fm_rawread(uint32_t offset, void *buf, uint32_t size);
int fw_fm_rawerasesector(uint32_t sector);

#define CXD56_SPIFLASHSIZE (16 * 1024 * 1024)

#define SECTOR_SHIFT 12
#define SECTOR_SIZE (1 << SECTOR_SHIFT)

#define PAGE_SHIFT 9
#define PAGE_SIZE (1 << PAGE_SHIFT)

#define NO_SECTOR 0xffffffff

uint8_t flash_cache[SECTOR_SIZE];
uint32_t flash_sector = NO_SECTOR;

void supervisor_flash_init(void) {
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return CXD56_SPIFLASHSIZE >> PAGE_SHIFT;
}

void port_internal_flash_flush(void) {
    if (flash_sector == NO_SECTOR) {
        return;
    }

    fw_fm_rawerasesector(flash_sector);
    fw_fm_rawwrite(flash_sector << SECTOR_SHIFT, flash_cache, SECTOR_SIZE);

    flash_sector = NO_SECTOR;
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    supervisor_flash_flush();

    if (fw_fm_rawread(block << PAGE_SHIFT, dest, num_blocks << PAGE_SHIFT) < 0) {
        return 1;
    }

    return 0; // success
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks) {
    uint32_t sector;
    uint32_t block_position;
    uint32_t current_block = 0;

    while (num_blocks--) {
        sector = (lba << PAGE_SHIFT) >> SECTOR_SHIFT;
        block_position = lba - 8 * sector;

        if (sector != flash_sector) {
            supervisor_flash_flush();

            if (fw_fm_rawread(sector << SECTOR_SHIFT, flash_cache, SECTOR_SIZE) < 0) {
                return 1;
            }

            if (memcmp(&flash_cache[block_position << PAGE_SHIFT], &src[current_block << PAGE_SHIFT], PAGE_SIZE) != 0) {
                flash_sector = sector;
            }
        }

        memcpy(&flash_cache[block_position << PAGE_SHIFT], &src[current_block << PAGE_SHIFT], PAGE_SIZE);

        lba++;
        current_block++;
    }

    return 0; // success
}

void supervisor_flash_release_cache(void) {
}
