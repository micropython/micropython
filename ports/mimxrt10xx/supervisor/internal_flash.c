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

#include "sdk/drivers/cache/armv7-m7/fsl_cache.h"
#include "sdk/drivers/flexspi/fsl_flexspi.h"
#include "fsl_iomuxc.h"

// defined in linker
extern uint32_t __fatfs_flash_start_addr[];
extern uint32_t __fatfs_flash_length[];

#define NO_CACHE        0xffffffff
#define SECTOR_SIZE     0x1000 /* 4K */

uint8_t _flash_cache[SECTOR_SIZE] __attribute__((aligned(4)));
uint32_t _flash_page_addr = NO_CACHE;

#ifndef FLEXSPI
#define FLEXSPI FLEXSPI1
#define FlexSPI_AMBA_BASE FlexSPI1_AMBA_BASE
#endif

void PLACE_IN_ITCM(supervisor_flash_init)(void) {
    // Update the LUT to make sure all entries are available. Copy the values to
    // memory first so that we don't read from the flash as we update the LUT.
    uint32_t lut_copy[64];
    memcpy(lut_copy, (const uint32_t *)&qspiflash_config.memConfig.lookupTable, 64 * sizeof(uint32_t));
    FLEXSPI_UpdateLUT(FLEXSPI, 0, lut_copy, 64);
    // Make sure everything is flushed after updating the LUT.
    __DSB();
    __ISB();
    flexspi_nor_init();

    #if IMXRT10XX
    // Disable interrupts of priority 8+. They likely use code in flash
    // itself. Higher priority interrupts (<8) should ensure all of their
    // code is in RAM.
    __set_BASEPRI(8 << (8 - __NVIC_PRIO_BITS));

    // Increase clock speed to 120 MHz
    // Wait for bus idle before change flash configuration.
    while (!FLEXSPI_GetBusIdleStatus(FLEXSPI)) {
    }
    FLEXSPI_Enable(FLEXSPI, false);

    // Disable FlexSPI clock
    CCM->CCGR6 &= ~CCM_CCGR6_CG5_MASK;

    // Changing the clock is OK now.

    // The PFD is 480 * 18 / PFD0_FRAC. We do / 18 which outputs 480 MHz.
    CCM_ANALOG->PFD_480 = (CCM_ANALOG->PFD_480 & ~CCM_ANALOG_PFD_480_TOG_PFD0_FRAC_MASK) | CCM_ANALOG_PFD_480_TOG_PFD0_FRAC(18);

    // This divides down the 480 Mhz by PODF + 1. So 480 / (3 + 1) = 120 MHz.
    CCM->CSCMR1 = (CCM->CSCMR1 & ~CCM_CSCMR1_FLEXSPI_PODF_MASK) | CCM_CSCMR1_FLEXSPI_PODF(3);

    // Re-enable FlexSPI
    CCM->CCGR6 |= CCM_CCGR6_CG5_MASK;

    FLEXSPI_Enable(FLEXSPI, true);

    FLEXSPI_SoftwareReset(FLEXSPI);

    while (!FLEXSPI_GetBusIdleStatus(FLEXSPI)) {
    }
    __set_BASEPRI(0U);
    #endif
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

        // Disable interrupts of priority 8+. They likely use code in flash
        // itself. Higher priority interrupts (<8) should ensure all of their
        // code is in RAM.
        __set_BASEPRI(8 << (8 - __NVIC_PRIO_BITS));
        status = flexspi_nor_flash_erase_sector(FLEXSPI, sector_addr);
        __set_BASEPRI(0U);
        if (status != kStatus_Success) {
            return;
        }

        for (int i = 0; i < SECTOR_SIZE / FLASH_PAGE_SIZE; ++i) {
            __set_BASEPRI(8 << (8 - __NVIC_PRIO_BITS));
            status = flexspi_nor_flash_page_program(FLEXSPI, sector_addr + i * FLASH_PAGE_SIZE, (void *)_flash_cache + i * FLASH_PAGE_SIZE);
            __set_BASEPRI(0U);
            if (status != kStatus_Success) {
                return;
            }
        }

        DCACHE_CleanInvalidateByRange(_flash_page_addr, SECTOR_SIZE);
    }
    _flash_page_addr = NO_CACHE;
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        uint32_t src = lba2addr(block + i);
        uint32_t page_addr = src & ~(SECTOR_SIZE - 1);
        // Copy from the cache if our page matches the cached one.
        if (page_addr == _flash_page_addr) {
            src = ((uint32_t)&_flash_cache) + (src - page_addr);
        }

        memcpy(dest + FILESYSTEM_BLOCK_SIZE * i, (uint8_t *)src, FILESYSTEM_BLOCK_SIZE);
    }

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

void PLACE_IN_ITCM(supervisor_flash_release_cache)(void) {
}
