/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
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
#if defined(RA4M1) | defined(RA4M3) | defined(RA4W1) | defined(RA6M1) | defined(RA6M2) | defined(RA6M3)
#include "ra_flash.h"
#endif

#if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE

// #define DEBUG_FLASH_BDEV

// Here we try to automatically configure the location and size of the flash
// pages to use for the internal storage.  We also configure the location of the
// cache used for writing.

#if defined(RA4M1)

STATIC byte flash_cache_mem[0x800] __attribute__((aligned(16))); // 2K
#define CACHE_MEM_START_ADDR (&flash_cache_mem[0])
#define FLASH_SECTOR_SIZE_MAX (0x800)           // 2k max
#define FLASH_MEM_SEG1_START_ADDR (0x37000)     // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (72)          // sectors 1,2,...,72

#elif defined(RA4M3)
STATIC byte flash_cache_mem[0x800] __attribute__((aligned(16))); // 2K
#define CACHE_MEM_START_ADDR (&flash_cache_mem[0])
#define FLASH_SECTOR_SIZE_MAX (0x800)           // 2k max
#define FLASH_MEM_SEG1_START_ADDR (0x37000)     // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (72)          // sectors 1,2,...,72

#elif defined(RA4W1)
STATIC byte flash_cache_mem[0x800] __attribute__((aligned(16))); // 2K
#define CACHE_MEM_START_ADDR (&flash_cache_mem[0])
#define FLASH_SECTOR_SIZE_MAX (0x800)           // 2k max
#define FLASH_MEM_SEG1_START_ADDR (0x70000)     // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (128)         // sectors 1,2,...,128

#elif defined(RA6M1)
STATIC byte flash_cache_mem[0x8000] __attribute__((aligned(16))); // 32K
#define CACHE_MEM_START_ADDR (&flash_cache_mem[0])
#define FLASH_SECTOR_SIZE_MAX (0x8000)          // 32k max
#define FLASH_MEM_SEG1_START_ADDR (0x60000)     // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (256)         // sectors 1,2,...,256

#elif defined(RA6M2)
STATIC byte flash_cache_mem[0x8000] __attribute__((aligned(16))); // 32K
#define CACHE_MEM_START_ADDR (&flash_cache_mem[0])
#define FLASH_SECTOR_SIZE_MAX (0x8000)          // 32k max
#define FLASH_MEM_SEG1_START_ADDR (0xe0000)     // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (512)         // sectors 1,2,...,512

#elif defined(RA6M3)
STATIC byte flash_cache_mem[0x8000] __attribute__((aligned(16))); // 32K
#define CACHE_MEM_START_ADDR (&flash_cache_mem[0])
#define FLASH_SECTOR_SIZE_MAX (0x8000)          // 32k max
#define FLASH_MEM_SEG1_START_ADDR (0x60000)     // sector 1
#define FLASH_MEM_SEG1_NUM_BLOCKS (256)         // sectors 1,2,...,256

#else
#error "no internal flash storage support for this MCU"
#endif

#if !defined(FLASH_MEM_SEG2_START_ADDR)
#define FLASH_MEM_SEG2_START_ADDR (0) // no second segment
#define FLASH_MEM_SEG2_NUM_BLOCKS (0) // no second segment
#endif

#define FLASH_FLAG_DIRTY        (1)
#define FLASH_FLAG_FORCE_WRITE  (2)
#define FLASH_FLAG_ERASED       (4)
static volatile uint8_t flash_flags = 0;
static uint32_t flash_cache_sector_id;
static uint32_t flash_cache_sector_start;
static uint32_t flash_cache_sector_size;
static long flash_tick_counter_last_write;

void flash_bdev_irq_handler(void);

int32_t flash_bdev_ioctl(uint32_t op, uint32_t arg) {
    (void)arg;
    switch (op) {
        case BDEV_IOCTL_INIT:
            flash_flags = 0;
            flash_cache_sector_id = 0;
            flash_tick_counter_last_write = 0L;
            return 0;

        case BDEV_IOCTL_NUM_BLOCKS:
            return FLASH_MEM_SEG1_NUM_BLOCKS + FLASH_MEM_SEG2_NUM_BLOCKS;

        case BDEV_IOCTL_IRQ_HANDLER:
            flash_bdev_irq_handler();
            return 0;

        case BDEV_IOCTL_SYNC:
            if (flash_flags & FLASH_FLAG_DIRTY) {
                flash_flags |= FLASH_FLAG_FORCE_WRITE;
                flash_bdev_irq_handler();
                // while (flash_flags & FLASH_FLAG_DIRTY) {
                //   NVIC->STIR = FLASH_IRQn;
                // }
            }
            return 0;
    }
    // return -MP_EINVAL;
    return -1;
}

static uint8_t *flash_cache_get_addr_for_write(uint32_t flash_addr) {
    uint32_t flash_sector_start;
    uint32_t flash_sector_size;
    uint32_t flash_sector_id = flash_get_sector_info(flash_addr, &flash_sector_start, &flash_sector_size);
    if (flash_sector_size > FLASH_SECTOR_SIZE_MAX) {
        flash_sector_size = FLASH_SECTOR_SIZE_MAX;
    }
    if (flash_cache_sector_id != flash_sector_id) {
        flash_bdev_ioctl(BDEV_IOCTL_SYNC, 0);
        memcpy((void *)CACHE_MEM_START_ADDR, (const void *)flash_sector_start, flash_sector_size);
        flash_cache_sector_id = flash_sector_id;
        flash_cache_sector_start = flash_sector_start;
        flash_cache_sector_size = flash_sector_size;
    }
    flash_flags |= FLASH_FLAG_DIRTY;
    led_state(RA_LED1, 1); // indicate a dirty cache with LED on
    flash_tick_counter_last_write = (long)HAL_GetTick();
    return (uint8_t *)CACHE_MEM_START_ADDR + flash_addr - flash_sector_start;
}

void flash_cache_commit(void) {
    if (flash_flags & FLASH_FLAG_DIRTY) {
        if (((long)HAL_GetTick() - flash_tick_counter_last_write) > 1000) {
            flash_bdev_irq_handler();
        }
    }
}

static uint8_t *flash_cache_get_addr_for_read(uint32_t flash_addr) {
    uint32_t flash_sector_start;
    uint32_t flash_sector_size;
    uint32_t flash_sector_id = flash_get_sector_info(flash_addr, &flash_sector_start, &flash_sector_size);
    if (flash_cache_sector_id == flash_sector_id) {
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
    if (block < FLASH_MEM_SEG1_NUM_BLOCKS + FLASH_MEM_SEG2_NUM_BLOCKS) {
        return FLASH_MEM_SEG2_START_ADDR + (block - FLASH_MEM_SEG1_NUM_BLOCKS) * FLASH_BLOCK_SIZE;
    }
    // can add more flash segments here if needed, following above pattern

    // bad block
    return -1;
}

void flash_bdev_irq_handler(void) {
    if (!(flash_flags & FLASH_FLAG_DIRTY)) {
        return;
    }

    // This code uses interrupts to erase the flash
    /*
    if (flash_erase_state == 0) {
        flash_erase_it(flash_cache_sector_start, flash_cache_sector_size / 4);
        flash_erase_state = 1;
        return;
    }

    if (flash_erase_state == 1) {
        // wait for erase
        // TODO add timeout
        #define flash_erase_done() (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) == RESET)
        if (!flash_erase_done()) {
            return;
        }
        flash_erase_state = 2;
    }
    */

    // This code erases the flash directly, waiting for it to finish
    if (!(flash_flags & FLASH_FLAG_ERASED)) {
        flash_erase(flash_cache_sector_start, flash_cache_sector_size);
        flash_flags |= FLASH_FLAG_ERASED;
        // return;
    }

    // If not a forced write, wait at least 5 seconds after last write to flush
    // On file close and flash unmount we get a forced write, so we can afford to wait a while
    if ((flash_flags & FLASH_FLAG_FORCE_WRITE) || ((long)HAL_GetTick() - flash_tick_counter_last_write) >= 3000L) {
        // sync the cache RAM buffer by writing it to the flash page
        flash_tick_counter_last_write = 0x7fffffffL;
        flash_write(flash_cache_sector_start, (const uint32_t *)CACHE_MEM_START_ADDR, flash_cache_sector_size);
        // clear the flash flags now that we have a clean cache
        flash_flags = 0;
        // indicate a clean cache with LED off
        led_state(RA_LED1, 0);
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

bool flash_bdev_is_erased(uint32_t block) {
    uint32_t *start;
    uint32_t *end;
    bool ret = true;
    uint32_t flash_addr = convert_block_to_flash_addr(block);
    start = (uint32_t *)flash_addr;
    end = (uint32_t *)(flash_addr + FLASH_BLOCK_SIZE);
    while (start < end) {
        if (*start++ != 0xffffffff) {
            ret = false;
            break;
        }
    }
    return ret;
}

bool flash_bdev_writeblock(const uint8_t *src, uint32_t block) {
    // non-MBR block, copy to cache
    uint32_t flash_addr = convert_block_to_flash_addr(block);
    if (flash_addr == -1) {
        // bad block number
        return false;
    }
    uint8_t *dest = flash_cache_get_addr_for_write(flash_addr);
    memcpy(dest, src, FLASH_BLOCK_SIZE);
    // flash_flags |= FLASH_FLAG_FORCE_WRITE;
    // flash_bdev_irq_handler();
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
//        uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
        uint8_t *dest = flash_cache_get_addr_for_write(flash_addr + offset);
        memcpy(dest, src, l);
//        restore_irq_pri(basepri);
        src += l;
        block += 1;
        offset = 0;
        len -= l;
    }
    return 0;
}

#endif // MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
