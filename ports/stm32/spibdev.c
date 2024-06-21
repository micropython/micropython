/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Damien P. George
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

#include "py/obj.h"
#include "py/mperrno.h"
#include "irq.h"
#include "led.h"
#include "storage.h"
#include "drivers/memory/external_flash_device.h"

#if MICROPY_HW_ENABLE_STORAGE

int32_t spi_bdev_ioctl(spi_bdev_t *bdev, uint32_t op, uint32_t arg) {
    switch (op) {
        case BDEV_IOCTL_INIT:
            #ifdef MICROPY_HW_BDEV_SPIFLASH_CONFIG
            if (!arg) {
                arg = (uint32_t)(MICROPY_HW_BDEV_SPIFLASH_CONFIG);
            }
            #endif
            bdev->spiflash.config = (const mp_spiflash_config_t *)arg;
            int ret = mp_spiflash_init(&bdev->spiflash);
            bdev->flash_tick_counter_last_write = 0;
            return ret;

        case BDEV_IOCTL_NUM_BLOCKS:
            #if MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES
            return MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES / FLASH_BLOCK_SIZE;
            #else
            if (bdev->spiflash.device != NULL) {
                external_flash_device *flash = (external_flash_device *)bdev->spiflash.device;
                return flash->total_size / FLASH_BLOCK_SIZE;
            }
            return -1;
            #endif

        case BDEV_IOCTL_IRQ_HANDLER: {
            int ret = 0;
            #if MICROPY_HW_SPIFLASH_ENABLE_CACHE
            if ((bdev->spiflash.flags & 1) && HAL_GetTick() - bdev->flash_tick_counter_last_write >= 1000) {
                ret = mp_spiflash_cache_flush(&bdev->spiflash);
                led_state(PYB_LED_RED, 0); // indicate a clean cache with LED off
            }
            #endif
            return ret;
        }

        case BDEV_IOCTL_SYNC: {
            int ret = 0;
            #if MICROPY_HW_SPIFLASH_ENABLE_CACHE
            if (bdev->spiflash.flags & 1) {
                uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
                ret = mp_spiflash_cache_flush(&bdev->spiflash);
                led_state(PYB_LED_RED, 0); // indicate a clean cache with LED off
                restore_irq_pri(basepri);
            }
            #endif
            return ret;
        }
    }
    return -MP_EINVAL;
}

#if MICROPY_HW_SPIFLASH_ENABLE_CACHE
int spi_bdev_readblocks(spi_bdev_t *bdev, uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
    int ret = mp_spiflash_cached_read(&bdev->spiflash, block_num * FLASH_BLOCK_SIZE, num_blocks * FLASH_BLOCK_SIZE, dest);
    restore_irq_pri(basepri);

    return ret;
}

int spi_bdev_writeblocks(spi_bdev_t *bdev, const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
    int ret = mp_spiflash_cached_write(&bdev->spiflash, block_num * FLASH_BLOCK_SIZE, num_blocks * FLASH_BLOCK_SIZE, src);
    if (bdev->spiflash.flags & 1) {
        led_state(PYB_LED_RED, 1); // indicate a dirty cache with LED on
        bdev->flash_tick_counter_last_write = HAL_GetTick();
    }
    restore_irq_pri(basepri);

    return ret;
}
#endif // MICROPY_HW_SPIFLASH_ENABLE_CACHE

int spi_bdev_readblocks_raw(spi_bdev_t *bdev, uint8_t *dest, uint32_t block_num, uint32_t block_offset, uint32_t num_bytes) {
    uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
    int ret = mp_spiflash_read(&bdev->spiflash, block_num * MP_SPIFLASH_ERASE_BLOCK_SIZE + block_offset, num_bytes, dest);
    restore_irq_pri(basepri);

    return ret;
}

int spi_bdev_writeblocks_raw(spi_bdev_t *bdev, const uint8_t *src, uint32_t block_num, uint32_t block_offset, uint32_t num_bytes) {
    uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
    int ret = mp_spiflash_write(&bdev->spiflash, block_num * MP_SPIFLASH_ERASE_BLOCK_SIZE + block_offset, num_bytes, src);
    restore_irq_pri(basepri);

    return ret;
}

int spi_bdev_eraseblocks_raw(spi_bdev_t *bdev, uint32_t block_num, uint32_t num_bytes) {
    int ret = 0;
    while (num_bytes >= MP_SPIFLASH_ERASE_BLOCK_SIZE) {
        uint32_t basepri = raise_irq_pri(IRQ_PRI_FLASH); // prevent cache flushing and USB access
        ret = mp_spiflash_erase_block(&bdev->spiflash, block_num * MP_SPIFLASH_ERASE_BLOCK_SIZE);
        restore_irq_pri(basepri);
        if (ret) {
            break;
        }
        block_num += 1;
        num_bytes -= MP_SPIFLASH_ERASE_BLOCK_SIZE;
    }
    return ret;
}

#endif
