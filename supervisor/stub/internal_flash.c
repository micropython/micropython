// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT
#include "supervisor/flash.h"

#include <stdint.h>
#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"

void supervisor_flash_init(void) {
}

uint32_t supervisor_flash_get_block_size(void) {
    return 0;
}

uint32_t supervisor_flash_get_block_count(void) {
    return 0;
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    return 0; // success
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    return 0; // success
}

#if (0)
// See definition in supervisor/flash.c
void supervisor_flash_init_vfs(struct _fs_user_mount_t *vfs) {
    return;
}

// See definition in supervisor/flash.c
void supervisor_flash_flush(void) {
    return;
}
#endif

void supervisor_flash_release_cache(void) {
}

void port_internal_flash_flush(void) {
    return;
}
