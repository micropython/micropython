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

#include "supervisor/internal_flash.h"

#include <stdint.h>
#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif
#ifdef SAME51
#include "hri/hri_mclk_e51.h"
#endif
#ifdef SAME54
#include "hri/hri_mclk_e54.h"
#endif
#ifdef SAMD51
#include "hri/hri_mclk_d51.h"
#endif
#include "hal/include/hal_flash.h"

#include "supervisor/flash.h"

static struct flash_descriptor supervisor_flash_desc;

void supervisor_flash_init(void) {
    #ifdef SAM_D5X_E5X
    hri_mclk_set_AHBMASK_NVMCTRL_bit(MCLK);
    #endif
    #ifdef SAMD21
    _pm_enable_bus_clock(PM_BUS_APBB, NVMCTRL);
    #endif
    flash_init(&supervisor_flash_desc, NVMCTRL);
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return INTERNAL_FLASH_PART1_NUM_BLOCKS;
}

void port_internal_flash_flush(void) {
}

void supervisor_flash_release_cache(void) {
}

void flash_flush(void) {
    supervisor_flash_flush();
}

static int32_t convert_block_to_flash_addr(uint32_t block) {
    if (0 <= block && block < INTERNAL_FLASH_PART1_NUM_BLOCKS) {
        // a block in partition 1
        return CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR + block * FILESYSTEM_BLOCK_SIZE;
    }
    // bad block
    return -1;
}

bool supervisor_flash_read_block(uint8_t *dest, uint32_t block) {
    // non-MBR block, get data from flash memory
    int32_t src = convert_block_to_flash_addr(block);
    if (src == -1) {
        // bad block number
        return false;
    }
    int32_t error_code = flash_read(&supervisor_flash_desc, src, dest, FILESYSTEM_BLOCK_SIZE);
    return error_code == ERR_NONE;
}

bool supervisor_flash_write_block(const uint8_t *src, uint32_t block) {
    // non-MBR block, copy to cache
    int32_t dest = convert_block_to_flash_addr(block);
    if (dest == -1) {
        // bad block number
        return false;
    }
    int32_t error_code;
    error_code = flash_erase(&supervisor_flash_desc,
        dest,
        FILESYSTEM_BLOCK_SIZE / flash_get_page_size(&supervisor_flash_desc));
    if (error_code != ERR_NONE) {
        return false;
    }

    error_code = flash_append(&supervisor_flash_desc, dest, src, FILESYSTEM_BLOCK_SIZE);
    if (error_code != ERR_NONE) {
        return false;
    }
    return true;
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!supervisor_flash_read_block(dest + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    for (size_t i = 0; i < num_blocks; i++) {
        if (!supervisor_flash_write_block(src + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}
