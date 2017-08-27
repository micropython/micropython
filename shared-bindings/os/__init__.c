/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Josef Gajdusek
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include <string.h>

#include "extmod/vfs.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "py/mpstate.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/os/__init__.h"

//| :mod:`os` --- functions that an OS normally provides
//| ========================================================
//|
//| .. module:: os
//|   :synopsis: functions that an OS normally provides
//|   :platform: SAMD21
//|
//| The `os` module is a strict subset of the CPython `cpython:os` module. So,
//| code written in CircuitPython will work in CPython but not necessarily the
//| other way around.
//|

//| .. function:: uname()
//|
//|   Returns a named tuple of operating specific and CircuitPython port
//|   specific information.
//|
STATIC mp_obj_t os_uname(void) {
    return common_hal_os_uname();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_uname_obj, os_uname);

//| .. function:: chdir(path)
//|
//|   Change current directory.
//|
mp_obj_t os_chdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    common_hal_os_chdir(path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(os_chdir_obj, os_chdir);

//| .. function:: getcwd()
//|
//|   Get the current directory.
//|
mp_obj_t os_getcwd(void) {
    return common_hal_os_getcwd();
}
MP_DEFINE_CONST_FUN_OBJ_0(os_getcwd_obj, os_getcwd);

//| .. function:: listdir([dir])
//|
//|   With no argument, list the current directory.  Otherwise list the given directory.
//|
mp_obj_t os_listdir(size_t n_args, const mp_obj_t *args) {
    const char* path;
    if (n_args == 1) {
        path = mp_obj_str_get_str(args[0]);
    } else {
        path = "";
    }
    return common_hal_os_listdir(path);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(os_listdir_obj, 0, 1, os_listdir);

//| .. function:: mkdir(path)
//|
//|   Create a new directory.
//|
mp_obj_t os_mkdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    common_hal_os_mkdir(path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(os_mkdir_obj, os_mkdir);

//| .. function:: remove(path)
//|
//|   Remove a file.
//|
mp_obj_t os_remove(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    common_hal_os_remove(path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(os_remove_obj, os_remove);

//| .. function:: rmdir(path)
//|
//|   Remove a directory.
//|
mp_obj_t os_rename(mp_obj_t old_path_in, mp_obj_t new_path_in) {
    const char *old_path = mp_obj_str_get_str(old_path_in);
    const char *new_path = mp_obj_str_get_str(new_path_in);
    common_hal_os_rename(old_path, new_path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(os_rename_obj, os_rename);

//| .. function:: rename(old_path, new_path)
//|
//|   Rename a file.
//|
mp_obj_t os_rmdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    common_hal_os_rmdir(path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(os_rmdir_obj, os_rmdir);

//| .. function:: stat(path)
//|
//|   Get the status of a file or directory.
//|
mp_obj_t os_stat(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    return common_hal_os_stat(path);
}
MP_DEFINE_CONST_FUN_OBJ_1(os_stat_obj, os_stat);

//| .. function:: statvfs(path)
//|
//|   Get the status of a fileystem.
//|
//|   Returns a tuple with the filesystem information in the following order:
//|
//|        * ``f_bsize`` -- file system block size
//|        * ``f_frsize`` -- fragment size
//|        * ``f_blocks`` -- size of fs in f_frsize units
//|        * ``f_bfree`` -- number of free blocks
//|        * ``f_bavail`` -- number of free blocks for unpriviliged users
//|        * ``f_files`` -- number of inodes
//|        * ``f_ffree`` -- number of free inodes
//|        * ``f_favail`` -- number of free inodes for unpriviliged users
//|        * ``f_flag`` -- mount flags
//|        * ``f_namemax`` -- maximum filename length
//|
//|   Parameters related to inodes: ``f_files``, ``f_ffree``, ``f_avail``
//|   and the ``f_flags`` parameter may return ``0`` as they can be unavailable
//|   in a port-specific implementation.
//|
mp_obj_t os_statvfs(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    return common_hal_os_statvfs(path);
}
MP_DEFINE_CONST_FUN_OBJ_1(os_statvfs_obj, os_statvfs);

//| .. function:: sync()
//|
//|   Sync all filesystems.
//|
STATIC mp_obj_t os_sync(void) {
    for (mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table); vfs != NULL; vfs = vfs->next) {
        // this assumes that vfs->obj is fs_user_mount_t with block device functions
        disk_ioctl(MP_OBJ_TO_PTR(vfs->obj), CTRL_SYNC, NULL);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(os_sync_obj, os_sync);

//| .. function:: urandom(size)
//|
//|   Returns a string of *size* random bytes based on a hardware True Random
//|   Number Generator. When not available, it will raise a NotImplementedError.
//|
STATIC mp_obj_t os_urandom(mp_obj_t size_in) {
    mp_int_t size = mp_obj_get_int(size_in);
    uint8_t tmp[size];
    if (!common_hal_os_urandom(tmp, size)) {
        mp_raise_NotImplementedError("");
    }
    return mp_obj_new_bytes(tmp, size);
}
MP_DEFINE_CONST_FUN_OBJ_1(os_urandom_obj, os_urandom);

STATIC const mp_rom_map_elem_t os_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_os) },

    { MP_ROM_QSTR(MP_QSTR_uname), MP_ROM_PTR(&os_uname_obj) },

    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&os_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&os_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&os_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&os_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&os_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&os_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&os_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&os_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&os_statvfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlink), MP_ROM_PTR(&os_remove_obj) }, // unlink aliases to remove

    { MP_ROM_QSTR(MP_QSTR_sync), MP_ROM_PTR(&os_sync_obj) },

    { MP_ROM_QSTR(MP_QSTR_urandom), MP_ROM_PTR(&os_urandom_obj) },

//| .. data:: sep
//|
//|   Separator used to delineate path components such as folder and file names.
//|
    { MP_ROM_QSTR(MP_QSTR_sep), MP_ROM_QSTR(MP_QSTR__slash_) },
};

STATIC MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t os_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&os_module_globals,
};
