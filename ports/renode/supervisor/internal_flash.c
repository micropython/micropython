// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/internal_flash.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "supervisor/flash.h"

#define FLASH_BASE 0x10000000
#define FLASH_SIZE 0x80000

void supervisor_flash_init(void) {
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return FLASH_SIZE / FILESYSTEM_BLOCK_SIZE;
}

void port_internal_flash_flush(void) {
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    memcpy(dest,
        (void *)(FLASH_BASE + block * FILESYSTEM_BLOCK_SIZE),
        num_blocks * FILESYSTEM_BLOCK_SIZE);
    return 0;
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks) {
    memcpy((void *)(FLASH_BASE + lba * FILESYSTEM_BLOCK_SIZE),
        src,
        num_blocks * FILESYSTEM_BLOCK_SIZE);

    return 0; // success
}

void supervisor_flash_release_cache(void) {
}
