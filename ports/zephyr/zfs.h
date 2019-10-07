/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, NXP
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

#ifndef MICROPY_INCLUDED_ZEPHYR_ZFS_H
#define MICROPY_INCLUDED_ZEPHYR_ZFS_H

#include <zephyr.h>
#include <fs/fs.h>

#include "py/obj.h"
#include "py/lexer.h"

#define ZFS_IFDIR (0x4000)
#define ZFS_IFREG (0x8000)

typedef struct _zfs_mount_t {
    mp_obj_base_t base;
    struct fs_mount_t fs_mount;
} zfs_mount_t;

typedef struct _zfs_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    bool is_str;
    struct fs_dir_t dir;
} zfs_ilistdir_it_t;

#ifdef CONFIG_FAT_FILESYSTEM_ELM
#include <ff.h>

extern const mp_obj_type_t zfs_fat_type;

typedef struct _zfs_fat_t {
    mp_obj_base_t base;
    FATFS fat_fs;
} zfs_fat_t;
#endif

#ifdef CONFIG_FILE_SYSTEM_LITTLEFS
#include <fs/littlefs.h>

extern const mp_obj_type_t zfs_little_type;

typedef struct _zfs_little_t {
    mp_obj_base_t base;
    struct fs_littlefs littlefs;
} zfs_little_t;
#endif

mp_import_stat_t zfs_import_stat(const char *path);

mp_obj_t zfs_ilistdir(size_t n_args, const mp_obj_t *args);
mp_obj_t zfs_listdir(size_t n_args, const mp_obj_t *args);
mp_obj_t zfs_mkdir(mp_obj_t path_in);
mp_obj_t zfs_remove(mp_obj_t path_in);
mp_obj_t zfs_rename(mp_obj_t old_path_in, mp_obj_t new_path_in);
mp_obj_t zfs_stat(mp_obj_t path_in);
mp_obj_t zfs_statvfs(mp_obj_t path_in);
mp_obj_t zfs_mount(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(zfs_ilistdir_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(zfs_listdir_obj);
MP_DECLARE_CONST_FUN_OBJ_1(zfs_mkdir_obj);
MP_DECLARE_CONST_FUN_OBJ_1(zfs_remove_obj);
MP_DECLARE_CONST_FUN_OBJ_2(zfs_rename_obj);
MP_DECLARE_CONST_FUN_OBJ_1(zfs_stat_obj);
MP_DECLARE_CONST_FUN_OBJ_1(zfs_statvfs_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(zfs_mount_obj);

#endif // MICROPY_INCLUDED_ZEPHYR_ZFS_H
