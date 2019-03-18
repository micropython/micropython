/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "nrf_nvmc.h"

#ifdef BLUETOOTH_SD
#include "ble_drv.h"
#include "nrf_sdm.h"
#endif

// defined in linker
extern uint32_t __fatfs_flash_start_addr[];
extern uint32_t __fatfs_flash_length[];

#define NO_CACHE        0xffffffff
#define FL_PAGE_SZ      4096

uint8_t  _flash_cache[FL_PAGE_SZ] __attribute__((aligned(4)));
uint32_t _flash_page_addr = NO_CACHE;


/*------------------------------------------------------------------*/
/* Internal Flash API
 *------------------------------------------------------------------*/
static inline uint32_t lba2addr(uint32_t block) {
    return ((uint32_t)__fatfs_flash_start_addr) + block * FILESYSTEM_BLOCK_SIZE;
}

void supervisor_flash_init(void) {
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return ((uint32_t) __fatfs_flash_length) / FILESYSTEM_BLOCK_SIZE ;
}

#ifdef BLUETOOTH_SD
STATIC void sd_flash_operation_start(void) {
    sd_flash_operation_status = SD_FLASH_OPERATION_IN_PROGRESS;
}

STATIC sd_flash_operation_status_t sd_flash_operation_wait_until_done(void) {
    while (sd_flash_operation_status == SD_FLASH_OPERATION_IN_PROGRESS) {
        sd_app_evt_wait();
    }
    return sd_flash_operation_status;
}
#endif

void supervisor_flash_flush(void) {
    if (_flash_page_addr == NO_CACHE) return;

    // Skip if data is the same
    if (memcmp(_flash_cache, (void *)_flash_page_addr, FL_PAGE_SZ) != 0) {

#ifdef BLUETOOTH_SD
        uint8_t sd_en = 0;
        (void) sd_softdevice_is_enabled(&sd_en);

        if (sd_en) {
            uint32_t err_code;
            sd_flash_operation_status_t status;

            sd_flash_operation_start();
            err_code = sd_flash_page_erase(_flash_page_addr / FL_PAGE_SZ);
            if (err_code != NRF_SUCCESS) {
                mp_raise_OSError_msg_varg(translate("Flash erase failed to start, err 0x%04x"), err_code);
            }
            status = sd_flash_operation_wait_until_done();
            if (status == SD_FLASH_OPERATION_ERROR) {
                mp_raise_OSError_msg(translate("Flash erase failed"));
            }

            // Divide a full page into parts, because writing a full page causes an assertion failure.
            // See https://devzone.nordicsemi.com/f/nordic-q-a/40088/sd_flash_write-cause-nrf_fault_id_sd_assert/
            const size_t BLOCK_PARTS = 2;
            size_t words_to_write = FL_PAGE_SZ / sizeof(uint32_t) / BLOCK_PARTS;
            for (size_t i = 0; i < BLOCK_PARTS; i++) {
                sd_flash_operation_start();
                err_code = sd_flash_write(((uint32_t *)_flash_page_addr) + i * words_to_write,
                                          (uint32_t *)_flash_cache + i * words_to_write,
                                          words_to_write);
                if (err_code != NRF_SUCCESS) {
                    mp_raise_OSError_msg_varg(translate("Flash write failed to start, err 0x%04x"), err_code);
                }
                status = sd_flash_operation_wait_until_done();
                if (status == SD_FLASH_OPERATION_ERROR) {
                    mp_raise_OSError_msg(translate("Flash write failed"));
                }
            }
        } else {
#endif
            nrf_nvmc_page_erase(_flash_page_addr);
            nrf_nvmc_write_words(_flash_page_addr, (uint32_t *)_flash_cache, FL_PAGE_SZ / sizeof(uint32_t));
#ifdef BLUETOOTH_SD
        }
#endif

    }
    _flash_page_addr = NO_CACHE;
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    // Must write out anything in cache before trying to read.
    supervisor_flash_flush();
    uint32_t src = lba2addr(block);
    memcpy(dest, (uint8_t*) src, FILESYSTEM_BLOCK_SIZE*num_blocks);
    return 0; // success
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks) {
    while (num_blocks) {
        uint32_t const addr      = lba2addr(lba);
        uint32_t const page_addr = addr & ~(FL_PAGE_SZ - 1);

        uint32_t count = 8 - (lba % 8); // up to page boundary
        count = MIN(num_blocks, count);

        if (page_addr != _flash_page_addr) {
            supervisor_flash_flush();

            _flash_page_addr = page_addr;
            // Copy the current contents of the entire page into the cache.
            memcpy(_flash_cache, (void *)page_addr, FL_PAGE_SZ);
        }

        // Overwrite part or all of the page cache with the src data.
        memcpy(_flash_cache + (addr & (FL_PAGE_SZ - 1)), src, count * FILESYSTEM_BLOCK_SIZE);

        // adjust for next run
        lba        += count;
        src        += count * FILESYSTEM_BLOCK_SIZE;
        num_blocks -= count;
    }

    return 0; // success
}
