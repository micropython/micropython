/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 SummerGift <zhangyuan@rt-thread.com>
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
#ifndef MICROPY_INCLUDED_PY_MODUOS_FILE_H
#define MICROPY_INCLUDED_PY_MODUOS_FILE_H

#include "py/lexer.h"
#include "py/obj.h"

// MicroPython's port-standardized versions of stat constants
#define MP_S_IFDIR (0x4000)
#define MP_S_IFREG (0x8000)

// constants for block protocol ioctl
#define BP_IOCTL_INIT           (1)
#define BP_IOCTL_DEINIT         (2)
#define BP_IOCTL_SYNC           (3)
#define BP_IOCTL_SEC_COUNT      (4)
#define BP_IOCTL_SEC_SIZE       (5)

mp_obj_t mp_posix_mount(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
mp_obj_t mp_posix_umount(mp_obj_t mnt_in);
mp_obj_t mp_posix_open(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
mp_obj_t mp_posix_chdir(mp_obj_t path_in);
mp_obj_t mp_posix_getcwd(void);
mp_obj_t mp_posix_listdir(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_posix_mkdir(mp_obj_t path_in);
mp_obj_t mp_posix_remove(uint n_args, const mp_obj_t *arg);
mp_obj_t mp_posix_rename(mp_obj_t old_path_in, mp_obj_t new_path_in);
mp_obj_t mp_posix_rmdir(uint n_args, const mp_obj_t *arg);
mp_obj_t mp_posix_stat(mp_obj_t path_in);
mp_obj_t mp_posix_statvfs(mp_obj_t path_in);
mp_obj_t mp_posix_file_crc32(mp_obj_t path_in);
mp_obj_t mp_posix_mkfs(size_t n_args, const mp_obj_t *args);

MP_DECLARE_CONST_FUN_OBJ_KW(mp_posix_mount_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_posix_umount_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(mp_posix_open_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_posix_chdir_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_posix_getcwd_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_posix_listdir_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_posix_mkdir_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR(mp_posix_remove_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_posix_rename_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR(mp_posix_rmdir_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_posix_stat_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_posix_statvfs_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_posix_file_crc32_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_posix_mkfs_obj);
#endif // MICROPY_INCLUDED_PY_MODUOS_FILE_H
