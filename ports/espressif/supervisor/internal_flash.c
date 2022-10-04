/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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
#include "supervisor/internal_flash.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"

#include "components/spi_flash/include/esp_partition.h"

#include "supervisor/flash.h"
#include "supervisor/usb.h"

STATIC const esp_partition_t *_partition[2];

// TODO: Split the caching out of supervisor/shared/external_flash so we can use it.
#define SECTOR_SIZE 4096
STATIC uint8_t _cache[SECTOR_SIZE];
STATIC uint32_t _cache_lba = 0xffffffff;

void supervisor_flash_init(void) {
    _partition[0] = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_FAT,
        NULL);
    _partition[1] = esp_partition_find_first(ESP_PARTITION_TYPE_APP,
        ESP_PARTITION_SUBTYPE_APP_OTA_1,
        NULL);
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return (_partition[0]->size + _partition[1]->size) / FILESYSTEM_BLOCK_SIZE;
}

void port_internal_flash_flush(void) {

}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    uint32_t offset = block * FILESYSTEM_BLOCK_SIZE;
    uint32_t read_total = num_blocks * FILESYSTEM_BLOCK_SIZE;

    if (offset > _partition[0]->size) {
        // only read from partition 1
        esp_partition_read(_partition[1], (offset - _partition[0]->size), dest, read_total);
    } else if ((offset + read_total) > _partition[0]->size) {
        // first read from partition 0, then partition 1
        uint32_t read_0 = _partition[0]->size - offset;
        uint32_t read_1 = read_total - read_0;
        esp_partition_read(_partition[0], offset, dest, read_0);
        esp_partition_read(_partition[1], 0, (dest + read_0), read_1);
    } else {
        // only read from partition 0
        esp_partition_read(_partition[0], offset, dest, read_total);
    }

    return 0;
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks) {
    uint32_t blocks_per_sector = SECTOR_SIZE / FILESYSTEM_BLOCK_SIZE;
    uint32_t block = 0;
    while (block < num_blocks) {
        uint32_t block_address = lba + block;
        uint32_t sector_offset = block_address / blocks_per_sector * SECTOR_SIZE;
        uint8_t block_offset = block_address % blocks_per_sector;

        if (_cache_lba != block_address) {
            supervisor_flash_read_blocks(_cache, sector_offset / FILESYSTEM_BLOCK_SIZE, blocks_per_sector);
            _cache_lba = sector_offset;
        }

        for (uint8_t b = block_offset; b < blocks_per_sector; b++) {
            // Stop copying after the last block.
            if (block >= num_blocks) {
                break;
            }
            memcpy(_cache + b * FILESYSTEM_BLOCK_SIZE,
                src + block * FILESYSTEM_BLOCK_SIZE,
                FILESYSTEM_BLOCK_SIZE);
            block++;
        }

        if (sector_offset > _partition[0]->size) {
            // only write to partition 1
            esp_partition_erase_range(_partition[1], sector_offset - _partition[0]->size, SECTOR_SIZE);
            esp_partition_write(_partition[1], sector_offset - _partition[0]->size, _cache, SECTOR_SIZE);
        } else if ((sector_offset + SECTOR_SIZE) > _partition[0]->size) {
            // first write to partition 0, then partition 1
            uint32_t write_0 = _partition[0]->size - sector_offset;
            uint32_t write_1 = SECTOR_SIZE - write_0;
            esp_partition_erase_range(_partition[0], sector_offset, write_0);
            esp_partition_write(_partition[0], sector_offset, _cache, write_0);
            esp_partition_erase_range(_partition[1], 0, write_1);
            esp_partition_write(_partition[1], 0, _cache + write_0, write_1);
        } else {
            // only write to partition 0
            esp_partition_erase_range(_partition[0], sector_offset, SECTOR_SIZE);
            esp_partition_write(_partition[0], sector_offset, _cache, SECTOR_SIZE);
        }
    }

    return 0; // success
}

void supervisor_flash_release_cache(void) {
}
