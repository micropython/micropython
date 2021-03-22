/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#include "supervisor/flash.h"

#include <stdint.h>
#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "supervisor/shared/safe_mode.h"

#include "peripherals/nrf/nvm.h"

#ifdef BLUETOOTH_SD
#include "ble_drv.h"
#include "nrf_sdm.h"
#endif

#define NO_CACHE        0xffffffff

uint8_t _flash_cache[FLASH_PAGE_SIZE] __attribute__((aligned(4)));
uint32_t _flash_page_addr = NO_CACHE;


/*------------------------------------------------------------------*/
/* Internal Flash API
 *------------------------------------------------------------------*/
static inline uint32_t lba2addr(uint32_t block) {
    return CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR + block * FILESYSTEM_BLOCK_SIZE;
}

void supervisor_flash_init(void) {
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE / FILESYSTEM_BLOCK_SIZE;
}

void port_internal_flash_flush(void) {
    if (_flash_page_addr == NO_CACHE) {
        return;
    }

    // Skip if data is the same
    if (memcmp(_flash_cache, (void *)_flash_page_addr, FLASH_PAGE_SIZE) != 0) {
        if (!nrf_nvm_safe_flash_page_write(_flash_page_addr, _flash_cache)) {
            reset_into_safe_mode(FLASH_WRITE_FAIL);
        }
    }
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    // Must write out anything in cache before trying to read.
    supervisor_flash_flush();

    uint32_t src = lba2addr(block);
    memcpy(dest, (uint8_t *)src, FILESYSTEM_BLOCK_SIZE * num_blocks);
    return 0; // success
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks) {
    while (num_blocks) {
        uint32_t const addr = lba2addr(lba);
        uint32_t const page_addr = addr & ~(FLASH_PAGE_SIZE - 1);

        uint32_t count = 8 - (lba % 8); // up to page boundary
        count = MIN(num_blocks, count);

        if (page_addr != _flash_page_addr) {
            // Write out anything in cache before overwriting it.
            supervisor_flash_flush();

            _flash_page_addr = page_addr;

            // Copy the current contents of the entire page into the cache.
            memcpy(_flash_cache, (void *)page_addr, FLASH_PAGE_SIZE);
        }

        // Overwrite part or all of the page cache with the src data.
        memcpy(_flash_cache + (addr & (FLASH_PAGE_SIZE - 1)), src, count * FILESYSTEM_BLOCK_SIZE);

        // adjust for next run
        lba += count;
        src += count * FILESYSTEM_BLOCK_SIZE;
        num_blocks -= count;
    }

    return 0; // success
}

void supervisor_flash_release_cache(void) {
}
