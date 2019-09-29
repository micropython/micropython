/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"

#include "stm32f4xx_hal.h"

typedef struct {
    uint32_t base_address;
    uint32_t sector_size;
    uint32_t sector_count;
} flash_layout_t;

/*------------------------------------------------------------------*/
/* Internal Flash API
 *------------------------------------------------------------------*/

static const flash_layout_t flash_layout[] = {
    { 0x08000000, 0x04000, 4 },
    { 0x08010000, 0x10000, 1 },
    { 0x08020000, 0x20000, 3 },
    #if defined(FLASH_SECTOR_8)
    { 0x08080000, 0x20000, 4 },
    #endif
    #if defined(FLASH_SECTOR_12)
    { 0x08100000, 0x04000, 4 },
    { 0x08110000, 0x10000, 1 },
    { 0x08120000, 0x20000, 7 },
    #endif
};

static uint8_t sector_copy[0x4000] __attribute__((aligned(4)));

//Return the sector of a given flash address. 
uint32_t flash_get_sector_info(uint32_t addr, uint32_t *start_addr, uint32_t *size) {
    if (addr >= flash_layout[0].base_address) {
        uint32_t sector_index = 0;
        for (uint8_t i = 0; i < MP_ARRAY_SIZE(flash_layout); ++i) {
            for (uint8_t j = 0; j < flash_layout[i].sector_count; ++j) {
                uint32_t sector_start_next = flash_layout[i].base_address
                    + (j + 1) * flash_layout[i].sector_size;
                if (addr < sector_start_next) {
                    if (start_addr != NULL) {
                        *start_addr = flash_layout[i].base_address
                            + j * flash_layout[i].sector_size;
                    }
                    if (size != NULL) {
                        *size = flash_layout[i].sector_size;
                    }
                    return sector_index;
                }
                ++sector_index;
            }
        }
    }
    return 0;
}

void supervisor_flash_init(void) {
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return INTERNAL_FLASH_FILESYSTEM_NUM_BLOCKS;
}

void supervisor_flash_flush(void) {
}

static int32_t convert_block_to_flash_addr(uint32_t block) {
    if (0 <= block && block < INTERNAL_FLASH_FILESYSTEM_NUM_BLOCKS) {
        // a block in partition 1
        return INTERNAL_FLASH_FILESYSTEM_START_ADDR + block * FILESYSTEM_BLOCK_SIZE;
    }
    // bad block
    return -1;
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    int32_t src = convert_block_to_flash_addr(block);
    if (src == -1) {
        // bad block number
        return false;
    }
    memcpy(dest, (uint8_t*) src, FILESYSTEM_BLOCK_SIZE*num_blocks);
    return 0; // success
}

bool supervisor_flash_write_block(const uint8_t *src, uint32_t block) {
    int32_t dest = convert_block_to_flash_addr(block);
    if (dest == -1) {
        // bad block number
        mp_printf(&mp_plat_print, "BAD FLASH BLOCK ERROR");
        return false;
    }

    // unlock flash
    HAL_FLASH_Unlock();

    // set up for erase
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = VOLTAGE_RANGE_3; // voltage range needs to be 2.7V to 3.6V
    // get the sector information
    uint32_t sector_size;
    uint32_t sector_start_addr;
    EraseInitStruct.Sector = flash_get_sector_info(dest, &sector_start_addr, &sector_size);
    EraseInitStruct.NbSectors = 1;
    if (sector_size>0x4000) return false;

    // copy the sector
    memcpy(sector_copy,(void *)sector_start_addr,sector_size);

    // // overwrite sector data
    memcpy(sector_copy+(dest-sector_start_addr),src,FILESYSTEM_BLOCK_SIZE);

    // find end address, subtract for number of sectors
    // Shouldn't be required since blocks will always fit in a single sector, they should never overlap
    //EraseInitStruct.NbSectors = flash_get_sector_info(dest + FILESYSTEM_BLOCK_SIZE - 1, NULL, NULL) - EraseInitStruct.Sector + 1;

    // erase the sector
    uint32_t SectorError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        // error occurred during sector erase
        HAL_FLASH_Lock(); // lock the flash
        mp_printf(&mp_plat_print, "FLASH SECTOR ERASE ERROR");
        return false;
    }

    __HAL_FLASH_DATA_CACHE_DISABLE();
    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

    __HAL_FLASH_DATA_CACHE_RESET();
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();

    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    __HAL_FLASH_DATA_CACHE_ENABLE();

    // reprogram the sector
    for (uint32_t i = 0; i < sector_size; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, sector_start_addr, (uint64_t)sector_copy[i]) != HAL_OK) {
            // error occurred during flash write
            HAL_FLASH_Lock(); // lock the flash
            mp_printf(&mp_plat_print, "FLASH WRITE ERROR");
            return false;
        }
        sector_start_addr += 1;
    }

    // lock the flash
    HAL_FLASH_Lock();

    return true;
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {

    for (size_t i = 0; i < num_blocks; i++) {
        if (!supervisor_flash_write_block(src + i * FILESYSTEM_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

void supervisor_flash_release_cache(void) {
}

