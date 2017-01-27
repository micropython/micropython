/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Damien P. George
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

#ifndef MICROPY_INCLUDED_EXTMOD_VFS_H
#define MICROPY_INCLUDED_EXTMOD_VFS_H

#include "py/lexer.h"
#include "py/obj.h"

typedef struct _mp_vfs_mount_t {
    const char *str; // mount point with leading /
    size_t len;
    mp_obj_t obj;
    struct _mp_vfs_mount_t *next;
} mp_vfs_mount_t;

mp_import_stat_t mp_vfs_import_stat(const char *path);
mp_obj_t mp_vfs_mount(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
mp_obj_t mp_vfs_umount(mp_obj_t mnt_in);
mp_obj_t mp_vfs_open(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
mp_obj_t mp_vfs_chdir(mp_obj_t path_in);
mp_obj_t mp_vfs_getcwd(void);
mp_obj_t mp_vfs_listdir(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_vfs_mkdir(mp_obj_t path_in);
mp_obj_t mp_vfs_remove(mp_obj_t path_in);
mp_obj_t mp_vfs_rename(mp_obj_t old_path_in, mp_obj_t new_path_in);
mp_obj_t mp_vfs_rmdir(mp_obj_t path_in);
mp_obj_t mp_vfs_stat(mp_obj_t path_in);
mp_obj_t mp_vfs_statvfs(mp_obj_t path_in);

MP_DECLARE_CONST_FUN_OBJ_KW(mp_vfs_mount_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_vfs_umount_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(mp_vfs_open_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_vfs_chdir_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_vfs_getcwd_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_vfs_listdir_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_vfs_mkdir_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_vfs_remove_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_vfs_rename_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_vfs_rmdir_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_vfs_stat_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_vfs_statvfs_obj);

#endif // MICROPY_INCLUDED_EXTMOD_VFS_H
