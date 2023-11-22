/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
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

#include <stdint.h>
#include <string.h>

#include "py/obj.h"
#include "py/mperrno.h"
#include "irq.h"
#include "led.h"
#include "flash.h"
#include "storage.h"

#if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE

// The linker script specifies flash storage and RAM cache locations.
extern uint8_t _micropy_hw_internal_flash_storage_start;
extern uint8_t _micropy_hw_internal_flash_storage_end;
extern uint8_t _micropy_hw_internal_flash_storage2_start;
extern uint8_t _micropy_hw_internal_flash_storage2_end;
extern uint8_t _micropy_hw_internal_flash_storage_ram_cache_start[];
extern uint8_t _micropy_hw_internal_flash_storage_ram_cache_end[];

#define CACHE_MEM_START_ADDR \
    ((uintptr_t)&_micropy_hw_internal_flash_storage_ram_cache_start[0])
#define FLASH_SECTOR_SIZE_MAX \
    (&_micropy_hw_internal_flash_storage_ram_cache_end[0] - &_micropy_hw_internal_flash_storage_ram_cache_start[0])
#define FLASH_MEM_SEG1_START_ADDR \
    ((long)&_micropy_hw_internal_flash_storage_start)
#define FLASH_MEM_SEG1_NUM_BLOCKS \
    ((&_micropy_hw_internal_flash_storage_end - &_micropy_hw_internal_flash_storage_start) / 512)

#if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE_SEGMENT2
#define FLASH_MEM_SEG2_START_ADDR \
    ((long)&_micropy_hw_internal_flash_storage2_start)
#define FLASH_MEM_SEG2_NUM_BLOCKS \
    ((&_micropy_hw_internal_flash_storage2_end - &_micropy_hw_internal_flash_storage2_start) / 512)
#endif

#if !defined(FLASH_MEM_SEG2_START_ADDR)
#define FLASH_MEM_SEG2_NUM_BLOCKS (0) // no second segment
#endif

#define FLASH_FLAG_DIRTY        (1)
#define FLASH_FLAG_FORCE_WRITE  (2)
#define FLASH_FLAG_ERASED       (4)
static __IO uint8_t flash_flags = 0;
static uint32_t flash_cache_sector_start;
static uint32_t flash_cache_sector_size;
static uint32_t flash_tick_counter_last_write;

static void flash_bdev_irq_handler(void);

int32_t flash_bdev_ioctl(uint32_t op, uint32_t arg) {
    (void)arg;
    switch (op) {
        case BDEV_IOCTL_INIT:
            flash_flags = 0;
            flash_cache_sector_start = (uint32_t)-1;
            flash_tick_counter_last_write = 0;
            return 0;

        case BDEV_IOCTL_NUM_BLOCKS:
            // Units are FLASH_BLOCK_SIZE
            return FLASH_MEM_SEG1_NUM_BLOCKS + FLASH_MEM_SEG2_NUM_BLOCKS;

        case BDEV_IOCTL_IRQ_HANDLER:
            flash_bdev_irq_handler();
            return 0;

        case BDEV_IOCTL_SYNC: {
            uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
            if (flash_flags & FLASH_FLAG_DIRTY) {
                flash_flags |= FLASH_FLAG_FORCE_WRITE;
                while (flash_flags & FLASH_FLAG_DIRTY) {
                    flash_bdev_irq_handler();
                }
            }
            restore_irq_pri(basepri);
            return 0;
        }
    }
    return -MP_EINVAL;
}

static uint8_t *flash_cache_get_addr_for_write(uint32_t flash_addr) {
    uint32_t flash_sector_start;
    uint32_t flash_sector_size;
    flash_get_sector_info(flash_addr, &flash_sector_start, &flash_sector_size);
    if (flash_sector_size > FLASH_SECTOR_SIZE_MAX) {
        flash_sector_size = FLASH_SECTOR_SIZE_MAX;
    }
    if (flash_cache_sector_start != flash_sector_start) {
        flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0);
        memcpy((void *)CACHE_MEM_START_ADDR, (const void *)flash_sector_start, flash_sector_size);
        flash_cache_sector_start = flash_sector_start;
        flash_cache_sector_size = flash_sector_size;
    }
    flash_flags |= FLASH_FLAG_DIRTY;
    led_state(PYB_LED_RED, 1); // indicate a dirty cache with LED on
    flash_tick_counter_last_write = HAL_GetTick();
    return (uint8_t *)CACHE_MEM_START_ADDR + flash_addr - flash_sector_start;
}

static uint8_t *flash_cache_get_addr_for_read(uint32_t flash_addr) {
    uint32_t flash_sector_start;
    uint32_t flash_sector_size;
    flash_get_sector_info(flash_addr, &flash_sector_start, &flash_sector_size);
    if (flash_cache_sector_start == flash_sector_start) {
        // in cache, copy from there
        return (uint8_t *)CACHE_MEM_START_ADDR + flash_addr - flash_sector_start;
    }
    // not in cache, copy straight from flash
    return (uint8_t *)flash_addr;
}

static uint32_t convert_block_to_flash_addr(uint32_t block) {
    if (block < FLASH_MEM_SEG1_NUM_BLOCKS) {
        return FLASH_MEM_SEG1_START_ADDR + block * FLASH_BLOCK_SIZE;
    }
    #ifdef FLASH_MEM_SEG2_START_ADDR
    if (block < FLASH_MEM_SEG1_NUM_BLOCKS + FLASH_MEM_SEG2_NUM_BLOCKS) {
        return FLASH_MEM_SEG2_START_ADDR + (block - FLASH_MEM_SEG1_NUM_BLOCKS) * FLASH_BLOCK_SIZE;
    }
    #endif
    // can add more flash segments here if needed, following above pattern

    // bad block
    return -1;
}

static void flash_bdev_irq_handler(void) {
    if (!(flash_flags & FLASH_FLAG_DIRTY)) {
        return;
    }

    // This code erases the flash directly, waiting for it to finish
    if (!(flash_flags & FLASH_FLAG_ERASED)) {
        flash_erase(flash_cache_sector_start);
        flash_flags |= FLASH_FLAG_ERASED;
        return;
    }

    // If not a forced write, wait at least 5 seconds after last write to flush
    // On file close and flash unmount we get a forced write, so we can afford to wait a while
    if ((flash_flags & FLASH_FLAG_FORCE_WRITE) || HAL_GetTick() - flash_tick_counter_last_write >= 5000) {
        // sync the cache RAM buffer by writing it to the flash page
        flash_write(flash_cache_sector_start, (const uint32_t *)CACHE_MEM_START_ADDR, flash_cache_sector_size / 4);
        // clear the flash flags now that we have a clean cache
        flash_flags = 0;
        // indicate a clean cache with LED off
        led_state(PYB_LED_RED, 0);
    }
}

bool flash_bdev_readblock(uint8_t *dest, uint32_t block) {
    // non-MBR block, get data from flash memory, possibly via cache
    uint32_t flash_addr = convert_block_to_flash_addr(block);
    if (flash_addr == -1) {
        // bad block number
        return false;
    }
    uint8_t *src = flash_cache_get_addr_for_read(flash_addr);
    memcpy(dest, src, FLASH_BLOCK_SIZE);
    return true;
}

bool flash_bdev_writeblock(const uint8_t *src, uint32_t block) {
    // non-MBR block, copy to cache
    uint32_t flash_addr = convert_block_to_flash_addr(block);
    if (flash_addr == -1) {
        // bad block number
        return false;
    }
    uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
    uint8_t *dest = flash_cache_get_addr_for_write(flash_addr);
    memcpy(dest, src, FLASH_BLOCK_SIZE);
    restore_irq_pri(basepri);
    return true;
}

int flash_bdev_readblocks_ext(uint8_t *dest, uint32_t block, uint32_t offset, uint32_t len) {
    // Get data from flash memory, possibly via cache
    while (len) {
        uint32_t l = MIN(len, FLASH_BLOCK_SIZE - offset);
        uint32_t flash_addr = convert_block_to_flash_addr(block);
        if (flash_addr == -1) {
            // bad block number
            return -1;
        }
        uint8_t *src = flash_cache_get_addr_for_read(flash_addr + offset);
        memcpy(dest, src, l);
        dest += l;
        block += 1;
        offset = 0;
        len -= l;
    }
    return 0;
}

int flash_bdev_writeblocks_ext(const uint8_t *src, uint32_t block, uint32_t offset, uint32_t len) {
    // Copy to cache
    while (len) {
        uint32_t l = MIN(len, FLASH_BLOCK_SIZE - offset);
        uint32_t flash_addr = convert_block_to_flash_addr(block);
        if (flash_addr == -1) {
            // bad block number
            return -1;
        }
        uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
        uint8_t *dest = flash_cache_get_addr_for_write(flash_addr + offset);
        memcpy(dest, src, l);
        restore_irq_pri(basepri);
        src += l;
        block += 1;
        offset = 0;
        len -= l;
    }
    return 0;
}

#endif // MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
