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

#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/timeutils/timeutils.h"
#include "extmod/vfs.h"
#include "extmod/vfs_lfs.h"

#if MICROPY_VFS && (MICROPY_VFS_LFS1 || MICROPY_VFS_LFS2)

enum { LFS_MAKE_ARG_bdev, LFS_MAKE_ARG_readsize, LFS_MAKE_ARG_progsize, LFS_MAKE_ARG_lookahead, LFS_MAKE_ARG_mtime };

static const mp_arg_t lfs_make_allowed_args[] = {
    { MP_QSTR_, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_readsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32} },
    { MP_QSTR_progsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32} },
    { MP_QSTR_lookahead, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32} },
    { MP_QSTR_mtime, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
};

#if MICROPY_VFS_LFS1

#include "lib/littlefs/lfs1.h"

#define LFS_BUILD_VERSION (1)
#define LFSx_MACRO(s) LFS1##s
#define LFSx_API(s) lfs1_##s
#define MP_VFS_LFSx(s) mp_vfs_lfs1_##s
#define MP_OBJ_VFS_LFSx mp_obj_vfs_lfs1_t
#define MP_OBJ_VFS_LFSx_FILE mp_obj_vfs_lfs1_file_t
#define MP_TYPE_VFS_LFSx mp_type_vfs_lfs1
#define MP_TYPE_VFS_LFSx_(s) mp_type_vfs_lfs1##s

typedef struct _mp_obj_vfs_lfs1_t {
    mp_obj_base_t base;
    mp_vfs_blockdev_t blockdev;
    vstr_t cur_dir;
    struct lfs1_config config;
    lfs1_t lfs;
} mp_obj_vfs_lfs1_t;

typedef struct _mp_obj_vfs_lfs1_file_t {
    mp_obj_base_t base;
    mp_obj_vfs_lfs1_t *vfs;
    lfs1_file_t file;
    struct lfs1_file_config cfg;
    uint8_t file_buffer[0];
} mp_obj_vfs_lfs1_file_t;

const char *mp_vfs_lfs1_make_path(mp_obj_vfs_lfs1_t *self, mp_obj_t path_in);
mp_obj_t mp_vfs_lfs1_file_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in);

#include "extmod/vfs_lfsx.c"
#include "extmod/vfs_lfsx_file.c"

#undef LFS_BUILD_VERSION
#undef LFSx_MACRO
#undef LFSx_API
#undef MP_VFS_LFSx
#undef MP_OBJ_VFS_LFSx
#undef MP_OBJ_VFS_LFSx_FILE
#undef MP_TYPE_VFS_LFSx
#undef MP_TYPE_VFS_LFSx_

#endif // MICROPY_VFS_LFS1

#if MICROPY_VFS_LFS2

#include "lib/littlefs/lfs2.h"

#define LFS_BUILD_VERSION (2)
#define LFSx_MACRO(s) LFS2##s
#define LFSx_API(s) lfs2_##s
#define MP_VFS_LFSx(s) mp_vfs_lfs2_##s
#define MP_OBJ_VFS_LFSx mp_obj_vfs_lfs2_t
#define MP_OBJ_VFS_LFSx_FILE mp_obj_vfs_lfs2_file_t
#define MP_TYPE_VFS_LFSx mp_type_vfs_lfs2
#define MP_TYPE_VFS_LFSx_(s) mp_type_vfs_lfs2##s

// Attribute ids for lfs2_attr.type.
#define LFS_ATTR_MTIME (1) // 64-bit little endian, nanoseconds since 1970/1/1

typedef struct _mp_obj_vfs_lfs2_t {
    mp_obj_base_t base;
    mp_vfs_blockdev_t blockdev;
    bool enable_mtime;
    vstr_t cur_dir;
    struct lfs2_config config;
    lfs2_t lfs;
} mp_obj_vfs_lfs2_t;

typedef struct _mp_obj_vfs_lfs2_file_t {
    mp_obj_base_t base;
    mp_obj_vfs_lfs2_t *vfs;
    uint8_t mtime[8];
    lfs2_file_t file;
    struct lfs2_file_config cfg;
    struct lfs2_attr attrs[1];
    uint8_t file_buffer[0];
} mp_obj_vfs_lfs2_file_t;

const char *mp_vfs_lfs2_make_path(mp_obj_vfs_lfs2_t *self, mp_obj_t path_in);
mp_obj_t mp_vfs_lfs2_file_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in);

STATIC void lfs_get_mtime(uint8_t buf[8]) {
    // On-disk storage of timestamps uses 1970 as the Epoch, so convert from host's Epoch.
    uint64_t ns = timeutils_nanoseconds_since_epoch_to_nanoseconds_since_1970(mp_hal_time_ns());
    // Store "ns" to "buf" in little-endian format (essentially htole64).
    for (size_t i = 0; i < 8; ++i) {
        buf[i] = ns;
        ns >>= 8;
    }
}

#include "extmod/vfs_lfsx.c"
#include "extmod/vfs_lfsx_file.c"

#endif // MICROPY_VFS_LFS2

#endif // MICROPY_VFS && (MICROPY_VFS_LFS1 || MICROPY_VFS_LFS2)
