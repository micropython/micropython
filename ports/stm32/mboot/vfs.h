/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_MBOOT_VFS_H
#define MICROPY_INCLUDED_STM32_MBOOT_VFS_H

#include "gzstream.h"
#include "mboot.h"

#if MBOOT_VFS_FAT

#include "lib/oofatfs/ff.h"

typedef struct _vfs_fat_context_t {
    uint32_t bdev_base_addr;
    uint32_t bdev_byte_len;
    FATFS fatfs;
    FIL fp;
} vfs_fat_context_t;

extern const stream_methods_t vfs_fat_stream_methods;

int vfs_fat_mount(vfs_fat_context_t *ctx, uint32_t base_addr, uint32_t byte_len);

#endif

#if MBOOT_VFS_LFS1

#include "lib/littlefs/lfs1.h"

#define LFS_READ_SIZE (32)
#define LFS_PROG_SIZE (32)
#define LFS_LOOKAHEAD_SIZE (32)

typedef struct _vfs_lfs1_context_t {
    uint32_t bdev_base_addr;
    struct lfs1_config config;
    lfs1_t lfs;
    struct lfs1_file_config filecfg;
    uint8_t filebuf[LFS_PROG_SIZE];
    lfs1_file_t file;
} vfs_lfs1_context_t;

extern const stream_methods_t vfs_lfs1_stream_methods;

int vfs_lfs1_mount(vfs_lfs1_context_t *ctx, uint32_t base_addr, uint32_t byte_len, uint32_t block_size);

#endif

#if MBOOT_VFS_LFS2

#include "lib/littlefs/lfs2.h"

#define LFS_READ_SIZE (32)
#define LFS_PROG_SIZE (32)
#define LFS_CACHE_SIZE (4 * LFS_READ_SIZE)
#define LFS_LOOKAHEAD_SIZE (32)

typedef struct _vfs_lfs2_context_t {
    uint32_t bdev_base_addr;
    struct lfs2_config config;
    lfs2_t lfs;
    struct lfs2_file_config filecfg;
    uint8_t filebuf[LFS_CACHE_SIZE]; // lfs2 specific
    lfs2_file_t file;
} vfs_lfs2_context_t;

extern const stream_methods_t vfs_lfs2_stream_methods;

int vfs_lfs2_mount(vfs_lfs2_context_t *ctx, uint32_t base_addr, uint32_t byte_len, uint32_t block_size);

#endif

#endif // MICROPY_INCLUDED_STM32_MBOOT_VFS_H
