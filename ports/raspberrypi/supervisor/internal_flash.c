/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/usb.h"

#include "src/rp2040/hardware_structs/include/hardware/structs/sio.h"
#include "src/rp2_common/hardware_flash/include/hardware/flash.h"
#include "src/common/pico_binary_info/include/pico/binary_info.h"

#define RESERVED_FLASH (1 * 1024 * 1024)

// TODO: Split the caching out of supervisor/shared/external_flash so we can use it.
#define SECTOR_SIZE 4096
#define NO_CACHE 0xffffffff
STATIC uint8_t _cache[SECTOR_SIZE];
STATIC uint32_t _cache_lba = NO_CACHE;
STATIC uint32_t _flash_size = 0;

void supervisor_flash_init(void) {
    bi_decl_if_func_used(bi_block_device(
        BINARY_INFO_MAKE_TAG('C', 'P'),
        "CircuitPython",
        RESERVED_FLASH,
        (1 * 1024 * 1024), // This is a minimum. We can't set it dynamically.
        NULL,
        BINARY_INFO_BLOCK_DEV_FLAG_READ |
        BINARY_INFO_BLOCK_DEV_FLAG_WRITE |
        BINARY_INFO_BLOCK_DEV_FLAG_PT_UNKNOWN));

    // Read the RDID register to get the flash capacity.
    uint8_t cmd[] = {0x9f, 0, 0, 0};
    uint8_t data[4];
    flash_do_cmd(cmd, data, 4);
    uint8_t power_of_two = 21;
    // Flash must be at least 2MB (1 << 21) because we use the first 1MB for the
    // CircuitPython core. We validate the range because Adesto Tech flash chips
    // don't return the correct value. So, we default to 2MB which will work for
    // larger chips, it just won't use all of the space.
    if (data[3] >= 21 && data[3] < 30) {
        power_of_two = data[3];
    }
    _flash_size = 1 << power_of_two;
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return (_flash_size - RESERVED_FLASH) / FILESYSTEM_BLOCK_SIZE;
}

void port_internal_flash_flush(void) {
    if (_cache_lba == NO_CACHE) {
        return;
    }
    common_hal_mcu_disable_interrupts();
    flash_range_erase(RESERVED_FLASH + _cache_lba, SECTOR_SIZE);
    flash_range_program(RESERVED_FLASH + _cache_lba, _cache, SECTOR_SIZE);
    common_hal_mcu_enable_interrupts();
    _cache_lba = NO_CACHE;
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    memcpy(dest,
        (void *)(XIP_BASE + RESERVED_FLASH + block * FILESYSTEM_BLOCK_SIZE),
        num_blocks * FILESYSTEM_BLOCK_SIZE);
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
            memcpy(_cache,
                (void *)(XIP_BASE + RESERVED_FLASH + sector_offset),
                SECTOR_SIZE);
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
        // Make sure we don't have an interrupt while we do flash operations.
        common_hal_mcu_disable_interrupts();
        flash_range_erase(RESERVED_FLASH + sector_offset, SECTOR_SIZE);
        flash_range_program(RESERVED_FLASH + sector_offset, _cache, SECTOR_SIZE);
        common_hal_mcu_enable_interrupts();
    }

    return 0; // success
}

void supervisor_flash_release_cache(void) {
}
