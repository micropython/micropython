/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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

#include "py/mpconfig.h"
#if MICROPY_VFS_FAT

#include "py/nlr.h"
#include "py/runtime.h"
#include "lib/fatfs/ff.h"
#include "lib/fatfs/diskio.h"
#include "extmod/vfs_fat_file.h"
#include "fsusermount.h"

#define mp_obj_fat_vfs_t fs_user_mount_t

STATIC mp_obj_t fat_vfs_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    mp_obj_fat_vfs_t *vfs = fatfs_mount_mkfs(n_args, args, (mp_map_t*)&mp_const_empty_map, false);
    vfs->base.type = type;
    return MP_OBJ_FROM_PTR(vfs);
}

STATIC mp_obj_t fat_vfs_mkfs(mp_obj_t bdev_in) {
    mp_obj_t args[] = {bdev_in, MP_OBJ_NEW_QSTR(MP_QSTR_mkfs)};
    fatfs_mount_mkfs(2, args, (mp_map_t*)&mp_const_empty_map, true);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fat_vfs_mkfs_fun_obj, fat_vfs_mkfs);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(fat_vfs_mkfs_obj, MP_ROM_PTR(&fat_vfs_mkfs_fun_obj));

STATIC mp_obj_t fat_vfs_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    // Skip self
    return fatfs_builtin_open(n_args - 1, args + 1, kwargs);
}
MP_DEFINE_CONST_FUN_OBJ_KW(fat_vfs_open_obj, 2, fat_vfs_open);

STATIC const mp_rom_map_elem_t fat_vfs_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mkfs), MP_ROM_PTR(&fat_vfs_mkfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&fat_vfs_open_obj) },
};
STATIC MP_DEFINE_CONST_DICT(fat_vfs_locals_dict, fat_vfs_locals_dict_table);

const mp_obj_type_t mp_fat_vfs_type = {
    { &mp_type_type },
    .name = MP_QSTR_VfsFat,
    .make_new = fat_vfs_make_new,
    .locals_dict = (mp_obj_dict_t*)&fat_vfs_locals_dict,
};

#endif // MICROPY_VFS_FAT
