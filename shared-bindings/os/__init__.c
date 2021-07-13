/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/os/__init__.h"

//| """functions that an OS normally provides
//|
//| The `os` module is a strict subset of the CPython `cpython:os` module. So,
//| code written in CircuitPython will work in CPython but not necessarily the
//| other way around."""
//|
//| import typing

//| def uname() -> _Uname:
//|     """Returns a named tuple of operating specific and CircuitPython port
//|     specific information."""
//|     ...
//|
//| class _Uname(typing.NamedTuple):
//|     """The type of values that :py:func:`.uname()` returns"""
//|
//|     sysname: str
//|     nodename: str
//|     release: str
//|     version: str
//|     machine: str
//|
STATIC mp_obj_t os_uname(void) {
    return common_hal_os_uname();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_uname_obj, os_uname);

//| def chdir(path: str) -> None:
//|     """Change current directory."""
//|     ...
//|
mp_obj_t os_chdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    common_hal_os_chdir(path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(os_chdir_obj, os_chdir);

//| def getcwd() -> str:
//|     """Get the current directory."""
//|     ...
//|
mp_obj_t os_getcwd(void) {
    return common_hal_os_getcwd();
}
MP_DEFINE_CONST_FUN_OBJ_0(os_getcwd_obj, os_getcwd);

//| def listdir(dir: str) -> str:
//|     """With no argument, list the current directory.  Otherwise list the given directory."""
//|     ...
//|
mp_obj_t os_listdir(size_t n_args, const mp_obj_t *args) {
    const char *path;
    if (n_args == 1) {
        path = mp_obj_str_get_str(args[0]);
    } else {
        path = mp_obj_str_get_str(common_hal_os_getcwd());
    }
    return common_hal_os_listdir(path);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(os_listdir_obj, 0, 1, os_listdir);

//| def mkdir(path: str) -> None:
//|     """Create a new directory."""
//|     ...
//|
mp_obj_t os_mkdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    common_hal_os_mkdir(path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(os_mkdir_obj, os_mkdir);

//| def remove(path: str) -> None:
//|     """Remove a file."""
//|     ...
//|
mp_obj_t os_remove(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    common_hal_os_remove(path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(os_remove_obj, os_remove);

//| def rmdir(path: str) -> None:
//|     """Remove a directory."""
//|     ...
//|
mp_obj_t os_rename(mp_obj_t old_path_in, mp_obj_t new_path_in) {
    const char *old_path = mp_obj_str_get_str(old_path_in);
    const char *new_path = mp_obj_str_get_str(new_path_in);
    common_hal_os_rename(old_path, new_path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(os_rename_obj, os_rename);

//| def rename(old_path: str, new_path: str) -> str:
//|     """Rename a file."""
//|     ...
//|
mp_obj_t os_rmdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    common_hal_os_rmdir(path);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(os_rmdir_obj, os_rmdir);

//| def stat(path: str) -> Tuple[int, int, int, int, int, int, int, int, int, int]:
//|     """Get the status of a file or directory.
//|
//|     .. note:: On builds without long integers, the number of seconds
//|        for contemporary dates will not fit in a small integer.
//|        So the time fields return 946684800,
//|        which is the number of seconds corresponding to 1999-12-31."""
//|     ...
//|
mp_obj_t os_stat(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    return common_hal_os_stat(path);
}
MP_DEFINE_CONST_FUN_OBJ_1(os_stat_obj, os_stat);

//| def statvfs(path: str) -> Tuple[int, int, int, int, int, int, int, int, int, int]:
//|     """Get the status of a filesystem.
//|
//|     Returns a tuple with the filesystem information in the following order:
//|
//|          * ``f_bsize`` -- file system block size
//|          * ``f_frsize`` -- fragment size
//|          * ``f_blocks`` -- size of fs in f_frsize units
//|          * ``f_bfree`` -- number of free blocks
//|          * ``f_bavail`` -- number of free blocks for unprivileged users
//|          * ``f_files`` -- number of inodes
//|          * ``f_ffree`` -- number of free inodes
//|          * ``f_favail`` -- number of free inodes for unprivileged users
//|          * ``f_flag`` -- mount flags
//|          * ``f_namemax`` -- maximum filename length
//|
//|     Parameters related to inodes: ``f_files``, ``f_ffree``, ``f_avail``
//|     and the ``f_flags`` parameter may return ``0`` as they can be unavailable
//|     in a port-specific implementation."""
//|     ...
//|
mp_obj_t os_statvfs(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    return common_hal_os_statvfs(path);
}
MP_DEFINE_CONST_FUN_OBJ_1(os_statvfs_obj, os_statvfs);

//| def sync() -> None:
//|     """Sync all filesystems."""
//|     ...
//|
STATIC mp_obj_t os_sync(void) {
    for (mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table); vfs != NULL; vfs = vfs->next) {
        // this assumes that vfs->obj is fs_user_mount_t with block device functions
        disk_ioctl(MP_OBJ_TO_PTR(vfs->obj), CTRL_SYNC, NULL);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(os_sync_obj, os_sync);

//| def urandom(size: int) -> str:
//|     """Returns a string of *size* random bytes based on a hardware True Random
//|     Number Generator. When not available, it will raise a NotImplementedError."""
//|     ...
//|
STATIC mp_obj_t os_urandom(mp_obj_t size_in) {
    mp_int_t size = mp_obj_get_int(size_in);
    mp_obj_str_t *result = MP_OBJ_TO_PTR(mp_obj_new_bytes_of_zeros(size));
    if (!common_hal_os_urandom((uint8_t *)result->data, size)) {
        mp_raise_NotImplementedError(translate("No hardware random available"));
    }
    return result;
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

//|
//| sep: str
//| """Separator used to delineate path components such as folder and file names."""
//|
    { MP_ROM_QSTR(MP_QSTR_sep), MP_ROM_QSTR(MP_QSTR__slash_) },
};

STATIC MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t os_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&os_module_globals,
};
