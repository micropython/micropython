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

#include "esp_ota_ops.h"
#include "esp_partition.h"

#include "supervisor/filesystem.h"
#include "supervisor/flash.h"
#include "supervisor/usb.h"

#define OP_READ     0
#define OP_WRITE    1

// TODO: Split the caching out of supervisor/shared/external_flash so we can use it.
#define SECTOR_SIZE 4096
STATIC uint8_t _cache[SECTOR_SIZE];
STATIC uint32_t _cache_lba = 0xffffffff;

#if CIRCUITPY_STORAGE_EXTEND
#if FF_MAX_SS == FF_MIN_SS
#define SECSIZE(fs) (FF_MIN_SS)
#else
#define SECSIZE(fs) ((fs)->ssize)
#endif // FF_MAX_SS == FF_MIN_SS
STATIC DWORD fatfs_bytes(void) {
    FATFS *fatfs = filesystem_circuitpy();
    return (fatfs->csize * SECSIZE(fatfs)) * (fatfs->n_fatent - 2);
}
STATIC bool storage_extended = true;
STATIC const esp_partition_t *_partition[2];
#else
STATIC const esp_partition_t *_partition[1];
#endif // CIRCUITPY_STORAGE_EXTEND

void supervisor_flash_init(void) {
    if (_partition[0] != NULL) {
        return;
    }
    _partition[0] = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_FAT,
        NULL);
    #if CIRCUITPY_STORAGE_EXTEND
    _partition[1] = esp_ota_get_next_update_partition(NULL);
    #endif
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    #if CIRCUITPY_STORAGE_EXTEND
    return ((storage_extended) ? (_partition[0]->size + _partition[1]->size) : _partition[0]->size) / FILESYSTEM_BLOCK_SIZE;
    #else
    return _partition[0]->size / FILESYSTEM_BLOCK_SIZE;
    #endif
}

void port_internal_flash_flush(void) {
}

STATIC void single_partition_rw(const esp_partition_t *partition, uint8_t *data,
    const uint32_t offset, const uint32_t size_total, const bool op) {
    if (op == OP_READ) {
        esp_partition_read(partition, offset, data, size_total);
    } else {
        esp_partition_erase_range(partition, offset, size_total);
        esp_partition_write(partition, offset, _cache, size_total);
    }
}

#if CIRCUITPY_STORAGE_EXTEND
STATIC void multi_partition_rw(uint8_t *data,
    const uint32_t offset, const uint32_t size_total, const bool op) {
    if (offset > _partition[0]->size) {
        // only r/w partition 1
        single_partition_rw(_partition[1], data, (offset - _partition[0]->size), size_total, op);
    } else if ((offset + size_total) > _partition[0]->size) {
        // first r/w partition 0, then partition 1
        uint32_t size_0 = _partition[0]->size - offset;
        uint32_t size_1 = size_total - size_0;
        if (op == OP_READ) {
            esp_partition_read(_partition[0], offset, data, size_0);
            esp_partition_read(_partition[1], 0, (data + size_0), size_1);
        } else {
            esp_partition_erase_range(_partition[0], offset, size_0);
            esp_partition_write(_partition[0], offset, _cache, size_0);
            esp_partition_erase_range(_partition[1], 0, size_1);
            esp_partition_write(_partition[1], 0, (_cache + size_0), size_1);
        }
    } else {
        // only r/w partition 0
        single_partition_rw(_partition[0], data, offset, size_total, op);
    }
}
#endif

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    const uint32_t offset = block * FILESYSTEM_BLOCK_SIZE;
    const uint32_t read_total = num_blocks * FILESYSTEM_BLOCK_SIZE;
    #if CIRCUITPY_STORAGE_EXTEND
    multi_partition_rw(dest, offset, read_total, OP_READ);
    #else
    single_partition_rw(_partition[0], dest, offset, read_total, OP_READ);
    #endif
    return 0; // success
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
        #if CIRCUITPY_STORAGE_EXTEND
        multi_partition_rw(_cache, sector_offset, SECTOR_SIZE, OP_WRITE);
        #else
        single_partition_rw(_partition[0], _cache, sector_offset, SECTOR_SIZE, OP_READ);
        #endif
    }
    return 0; // success
}

void supervisor_flash_release_cache(void) {
}

void supervisor_flash_set_extended(bool extended) {
    #if CIRCUITPY_STORAGE_EXTEND
    storage_extended = extended;
    #endif
}

bool supervisor_flash_get_extended(void) {
    #if CIRCUITPY_STORAGE_EXTEND
    return storage_extended;
    #else
    return false;
    #endif
}

void supervisor_flash_update_extended(void) {
    #if CIRCUITPY_STORAGE_EXTEND
    storage_extended = (_partition[0]->size < fatfs_bytes());
    #endif
}
