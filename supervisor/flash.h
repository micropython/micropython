/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_SUPERVISOR_FLASH_H
#define MICROPY_INCLUDED_SUPERVISOR_FLASH_H

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

#endif  // MICROPY_INCLUDED_SUPERVISOR_FLASH_H
