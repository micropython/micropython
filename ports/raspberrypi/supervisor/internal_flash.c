// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/internal_flash.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "genhdr/flash_info.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "audio_dma.h"
#include "supervisor/flash.h"
#include "supervisor/usb.h"

#ifdef PICO_RP2350
#include "src/rp2350/hardware_structs/include/hardware/structs/qmi.h"
#endif
#include "src/rp2040/hardware_structs/include/hardware/structs/sio.h"
#include "src/rp2_common/hardware_flash/include/hardware/flash.h"
#include "src/common/pico_binary_info/include/pico/binary_info.h"

#if !defined(TOTAL_FLASH_MINIMUM)
#define TOTAL_FLASH_MINIMUM (2 * 1024 * 1024)
#endif

// TODO: Split the caching out of supervisor/shared/external_flash so we can use it.
#define SECTOR_SIZE 4096
#define NO_CACHE 0xffffffff
static uint8_t _cache[SECTOR_SIZE];
static uint32_t _cache_lba = NO_CACHE;
static uint32_t _flash_size = 0;

#ifdef PICO_RP2350
static uint32_t m1_rfmt;
static uint32_t m1_timing;
#endif

static void save_psram_settings(void) {
    #ifdef PICO_RP2350
    // We're about to invalidate the XIP cache, clean it first to commit any dirty writes to PSRAM
    uint8_t *maintenance_ptr = (uint8_t *)XIP_MAINTENANCE_BASE;
    for (int i = 1; i < 16 * 1024; i += 8) {
        maintenance_ptr[i] = 0;
    }

    m1_timing = qmi_hw->m[1].timing;
    m1_rfmt = qmi_hw->m[1].rfmt;
    #endif
}

static void restore_psram_settings(void) {
    #ifdef PICO_RP2350
    qmi_hw->m[1].timing = m1_timing;
    qmi_hw->m[1].rfmt = m1_rfmt;
    #endif
}

void supervisor_flash_init(void) {
    bi_decl_if_func_used(bi_block_device(
        BINARY_INFO_MAKE_TAG('C', 'P'),
        "CircuitPython",
        CIRCUITPY_CIRCUITPY_DRIVE_START_ADDR,
        TOTAL_FLASH_MINIMUM - CIRCUITPY_CIRCUITPY_DRIVE_START_ADDR, // This is a minimum. We can't set it dynamically.
        NULL,
        BINARY_INFO_BLOCK_DEV_FLAG_READ |
        BINARY_INFO_BLOCK_DEV_FLAG_WRITE |
        BINARY_INFO_BLOCK_DEV_FLAG_PT_UNKNOWN));

    // Read the RDID register to get the flash capacity.
    uint8_t cmd[] = {0x9f, 0, 0, 0};
    uint8_t data[4];
    save_psram_settings();
    flash_do_cmd(cmd, data, 4);
    restore_psram_settings();
    uint8_t power_of_two = FLASH_DEFAULT_POWER_OF_TWO;
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
    return (_flash_size - CIRCUITPY_CIRCUITPY_DRIVE_START_ADDR) / FILESYSTEM_BLOCK_SIZE;
}

void port_internal_flash_flush(void) {
    if (_cache_lba == NO_CACHE) {
        return;
    }
    // Make sure we don't have an interrupt while we do flash operations.
    common_hal_mcu_disable_interrupts();
    // and audio DMA must be paused as well
    #if CIRCUITPY_AUDIOCORE
    uint32_t channel_mask = audio_dma_pause_all();
    #endif
    save_psram_settings();
    flash_range_erase(CIRCUITPY_CIRCUITPY_DRIVE_START_ADDR + _cache_lba, SECTOR_SIZE);
    flash_range_program(CIRCUITPY_CIRCUITPY_DRIVE_START_ADDR + _cache_lba, _cache, SECTOR_SIZE);
    restore_psram_settings();
    _cache_lba = NO_CACHE;
    #if CIRCUITPY_AUDIOCORE
    audio_dma_unpause_mask(channel_mask);
    #endif
    common_hal_mcu_enable_interrupts();
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    port_internal_flash_flush(); // we never read out of the cache, so we have to write it if dirty
    memcpy(dest,
        (void *)(XIP_BASE + CIRCUITPY_CIRCUITPY_DRIVE_START_ADDR + block * FILESYSTEM_BLOCK_SIZE),
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

        if (_cache_lba != sector_offset) {
            port_internal_flash_flush();
            memcpy(_cache,
                (void *)(XIP_BASE + CIRCUITPY_CIRCUITPY_DRIVE_START_ADDR + sector_offset),
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
    }

    return 0; // success
}

void supervisor_flash_release_cache(void) {
}
