// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/flash.h"
#include "supervisor/internal_flash.h"

#include <string.h>

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/sdioio/SDCard.h"

static sdioio_sdcard_obj_t sd;

static uint32_t first_lba = 0;
static uint32_t sector_count = 0;

static bool inited = false;

void supervisor_flash_init(void) {
    if (inited) {
        return;
    }
    inited = true;

    #if BCM_VERSION != 2711
    const mcu_pin_obj_t *data_pins[4] = {
        &pin_GPIO50,
        &pin_GPIO51,
        &pin_GPIO52,
        &pin_GPIO53,
    };
    common_hal_sdioio_sdcard_construct(&sd,
        &pin_GPIO48, &pin_GPIO49,
        4, data_pins, 8000000);
    #else
    common_hal_sdioio_sdcard_construct(&sd,
        NULL, NULL,
        0, NULL, 8000000);
    #endif
    common_hal_sdioio_sdcard_never_reset(&sd);

    uint32_t buffer[512 / sizeof(uint32_t)];
    mp_buffer_info_t bufinfo;
    bufinfo.len = 512;
    bufinfo.buf = buffer;
    common_hal_sdioio_sdcard_readblocks(&sd, 0, &bufinfo);
    size_t partition_count = 0;
    size_t first_free_sector = 0;
    for (size_t i = 0; i < 4; i++) {
        uint32_t part_first_lba = 0;
        uint32_t part_sector_count = 0;
        for (size_t j = 0; j < 16; j++) {
            size_t offset = 446 + i * 16 + j;
            uint8_t value = ((uint8_t *)buffer)[offset];
            // Little endian
            if (j >= 8 && j < 12) {
                part_first_lba |= value << (8 * (j - 8));
            } else if (j >= 12) {
                part_sector_count |= value << (8 * (j - 12));
            }
        }
        if (part_sector_count > 0) {
            partition_count += 1;
            first_free_sector = part_first_lba + part_sector_count;
        }
        // If the second partition exists, then use it (possibly after reformatting it.)
        if (i == 1 && part_sector_count > 0 && part_first_lba > 0) {
            sector_count = part_sector_count;
            first_lba = part_first_lba;
        } else if (i > 1 && part_sector_count > 0) {
            sector_count = 0;
        }
    }

    // Special case where most of the card is unused and the first and only
    // partition is likely the CP boot partition. (It is 255MB starting at 1MB)
    size_t sectors_per_mb = 2048;
    if (partition_count == 1 && first_free_sector == 256 * sectors_per_mb) {
        size_t total_sectors = common_hal_sdioio_sdcard_get_count(&sd);
        // Align to a MB
        total_sectors = total_sectors - (total_sectors % sectors_per_mb);
        // Don't use the last MB because the imager erases it. GPT tables put a
        // backup there sometimes.
        total_sectors -= sectors_per_mb;
        size_t new_sector_count = total_sectors - first_free_sector;

        // Modify the MBR
        size_t offset = 446 + 1 * 16;
        uint8_t *part = ((uint8_t *)buffer) + offset;
        part[0] = 0; // not boot
        part[1] = 0xff; // Old unused CHS sizing
        part[2] = 0xff;
        part[3] = 0xff;
        uint8_t type = 0x0C; // FAT32 with LBA
        if (new_sector_count >= 0x4000000) {
            type = 0x07; // Exfat when 32GB+
        }
        part[4] = type;
        part[5] = 0xff; // Old unused CHS sizing
        part[6] = 0xff;
        part[7] = 0xff;
        part[8] = first_free_sector & 0xff;
        part[9] = (first_free_sector >> 8) & 0xff;
        part[10] = (first_free_sector >> 16) & 0xff;
        part[11] = (first_free_sector >> 24) & 0xff;
        part[12] = new_sector_count & 0xff;
        part[13] = (new_sector_count >> 8) & 0xff;
        part[14] = (new_sector_count >> 16) & 0xff;
        part[15] = (new_sector_count >> 24) & 0xff;

        // Write back the modified MBR with the new partition. CircuitPython can
        // detect a previously formatted FAT filesystem later if only the MBR and
        // BOOT partition was updated.
        if (common_hal_sdioio_sdcard_writeblocks(&sd, 0, &bufinfo) == 0) {
            sector_count = new_sector_count;
            first_lba = first_free_sector;
        }
    }
}

uint32_t supervisor_flash_get_block_size(void) {
    return 512;
}

uint32_t supervisor_flash_get_block_count(void) {
    return sector_count;
}

void port_internal_flash_flush(void) {
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    mp_buffer_info_t bufinfo;
    bufinfo.len = 512;
    for (size_t i = 0; i < num_blocks; i++) {
        bufinfo.buf = dest + 512 * i;
        if (common_hal_sdioio_sdcard_readblocks(&sd, first_lba + block + i, &bufinfo) != 0) {
            return 1; // error
        }
    }
    return 0; // success
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t block, uint32_t num_blocks) {
    mp_buffer_info_t bufinfo;
    bufinfo.len = 512;
    for (size_t i = 0; i < num_blocks; i++) {
        bufinfo.buf = (uint8_t *)src + 512 * i;
        if (common_hal_sdioio_sdcard_writeblocks(&sd, first_lba + block + i, &bufinfo) != 0) {
            return 1; // error
        }
    }
    return 0; // success
}

void supervisor_flash_release_cache(void) {
}
