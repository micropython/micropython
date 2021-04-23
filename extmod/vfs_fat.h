// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_EXTMOD_VFS_FAT_H
#define MICROPY_INCLUDED_EXTMOD_VFS_FAT_H

#include "py/obj.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"

typedef struct _fs_user_mount_t {
    mp_obj_base_t base;
    mp_vfs_blockdev_t blockdev;
    FATFS fatfs;
} fs_user_mount_t;

extern const byte fresult_to_errno_table[20];
extern const mp_obj_type_t mp_fat_vfs_type;
extern const mp_obj_type_t mp_type_vfs_fat_fileio;
extern const mp_obj_type_t mp_type_vfs_fat_textio;

MP_DECLARE_CONST_FUN_OBJ_3(fat_vfs_open_obj);

typedef struct _pyb_file_obj_t {
    mp_obj_base_t base;
    FIL fp;
} pyb_file_obj_t;

#endif  // MICROPY_INCLUDED_EXTMOD_VFS_FAT_H
