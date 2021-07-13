/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019 Artur Pacholec
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
#include <stdio.h>

#include "boards/flash_config.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"

#include "fsl_cache.h"
#include "fsl_flexspi.h"
#include "fsl_iomuxc.h"

// defined in linker
extern uint32_t __fatfs_flash_start_addr[];
extern uint32_t __fatfs_flash_length[];

#define NO_CACHE        0xffffffff
#define SECTOR_SIZE     0x1000 /* 4K */

uint8_t _flash_cache[SECTOR_SIZE] __attribute__((aligned(4)));
uint32_t _flash_page_addr = NO_CACHE;

extern status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address);
extern status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t dstAddr, const uint32_t *src);
extern status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base);

void PLACE_IN_ITCM(supervisor_flash_init)(void) {
    // Update the LUT to make sure all entries are available.
    FLEXSPI_UpdateLUT(FLEXSPI, 0, (const uint32_t *)&qspiflash_config.memConfig.lookupTable, 64);
}

static inline uint32_t lba2addr(uint32_t block) {
    return CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR + block * FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE / FILESYSTEM_BLOCK_SIZE;
}

void PLACE_IN_ITCM(port_internal_flash_flush)(void) {
    if (_flash_page_addr == NO_CACHE) {
        return;
    }
    status_t status;

    // Skip if data is the same
    if (memcmp(_flash_cache, (void *)_flash_page_addr, SECTOR_SIZE) != 0) {
        volatile uint32_t sector_addr = (_flash_page_addr - FlexSPI_AMBA_BASE);

        __disable_irq();
        status = flexspi_nor_flash_erase_sector(FLEXSPI, sector_addr);
        __enable_irq();
        if (status != kStatus_Success) {
            printf("Page erase failure %ld!\r\n", status);
            return;
        }

        for (int i = 0; i < SECTOR_SIZE / FLASH_PAGE_SIZE; ++i) {
            __disable_irq();
            status = flexspi_nor_flash_page_program(FLEXSPI, sector_addr + i * FLASH_PAGE_SIZE, (void *)_flash_cache + i * FLASH_PAGE_SIZE);
            __enable_irq();
            if (status != kStatus_Success) {
                printf("Page program failure %ld!\r\n", status);
                return;
            }
        }

        DCACHE_CleanInvalidateByRange(_flash_page_addr, SECTOR_SIZE);
    }
    _flash_page_addr = NO_CACHE;
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
        uint32_t const page_addr = addr & ~(SECTOR_SIZE - 1);

        uint32_t count = 8 - (lba % 8); // up to page boundary
        count = MIN(num_blocks, count);

        if (page_addr != _flash_page_addr) {
            // Write out anything in cache before overwriting it.
            supervisor_flash_flush();

            _flash_page_addr = page_addr;

            // Copy the current contents of the entire page into the cache.
            memcpy(_flash_cache, (void *)page_addr, SECTOR_SIZE);
        }

        // Overwrite part or all of the page cache with the src data.
        memcpy(_flash_cache + (addr & (SECTOR_SIZE - 1)), src, count * FILESYSTEM_BLOCK_SIZE);

        // adjust for next run
        lba += count;
        src += count * FILESYSTEM_BLOCK_SIZE;
        num_blocks -= count;
    }

    return 0; // success
}

void supervisor_flash_release_cache(void) {
}
