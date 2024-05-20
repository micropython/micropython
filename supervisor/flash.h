// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT
#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "py/mpconfig.h"

#if INTERNAL_FLASH_FILESYSTEM
#include "supervisor/shared/internal_flash.h"
#else
#include "supervisor/shared/external_flash/external_flash.h"
#endif

void supervisor_flash_init(void);
uint32_t supervisor_flash_get_block_size(void);
uint32_t supervisor_flash_get_block_count(void);

// these return 0 on success, non-zero on error
mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks);
mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks);

struct _fs_user_mount_t;
void supervisor_flash_init_vfs(struct _fs_user_mount_t *vfs);
void supervisor_flash_flush(void);
void supervisor_flash_release_cache(void);

void supervisor_flash_set_extended(bool extended);
bool supervisor_flash_get_extended(void);
void supervisor_flash_update_extended(void);
