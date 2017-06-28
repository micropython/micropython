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

#include "extmod/vfs_fat.h"
#include "py/obj.h"
#include "py/objnamedtuple.h"
#include "shared-bindings/storage/__init__.h"

//| :mod:`storage` --- storage management
//| ========================================================
//|
//| .. module:: storage
//|   :synopsis: storage management
//|   :platform: SAMD21
//|
//| The `storage` provides storage management functionality such as mounting and
//| unmounting which is typically handled by the operating system hosting Python.
//| CircuitPython does not have an OS, so this module provides this functionality
//| directly. Its based on MicroPython's `uos` module but deliberately separates
//| CPython compatible functionality (in `os`) from incompatible functionality
//| (in `storage`).
//|

//| .. function:: mount(filesystem, mount_path, \*, readonly=False)
//|
//|   Mounts the given filesystem object at the given path.
//|
//|   This is the CircuitPython analog to the UNIX ``mount`` command.
//|
mp_obj_t storage_mount(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_readonly };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_readonly, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_false} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the mount point
    const char *mnt_str = mp_obj_str_get_str(pos_args[1]);

    // Make sure we're given an object we can mount.
    // TODO(tannewt): Make sure we have all the methods we need to operating it
    // as a file system.
    mp_obj_t vfs_obj = pos_args[0];
    mp_obj_t dest[2];
    mp_load_method_maybe(vfs_obj, MP_QSTR_mount, dest);
    if (dest[0] == MP_OBJ_NULL) {
        mp_raise_ValueError("filesystem must provide mount method");
    }

    common_hal_storage_mount(vfs_obj, mnt_str, mp_obj_is_true(args[ARG_readonly].u_obj));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(storage_mount_obj, 2, storage_mount);

//| .. function:: umount(mount)
//|
//|   Unmounts the given filesystem object or if *mount* is a path, then unmount
//|   the filesystem mounted at that location.
//|
//|   This is the CircuitPython analog to the UNIX ``umount`` command.
//|
mp_obj_t storage_umount(mp_obj_t mnt_in) {
    if (MP_OBJ_IS_STR(mnt_in)) {
        common_hal_storage_umount_path(mp_obj_str_get_str(mnt_in));
    } else {
        common_hal_storage_umount_object(mnt_in);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(storage_umount_obj, storage_umount);

//| .. function:: remount(mount_path, readonly)
//|
//|   Remounts the given path with new parameters.
//|
mp_obj_t storage_remount(mp_obj_t mount_path, mp_obj_t readonly) {
    if (!MP_OBJ_IS_STR(mount_path)) {
        mp_raise_ValueError("mount_path must be string");
    }

    common_hal_storage_remount(mp_obj_str_get_str(mount_path),
                               mp_obj_is_true(readonly));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(storage_remount_obj, storage_remount);

STATIC const mp_rom_map_elem_t storage_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_storage) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_mount), MP_ROM_PTR(&storage_mount_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_umount), MP_ROM_PTR(&storage_umount_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remount), MP_ROM_PTR(&storage_remount_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_VfsFat), MP_ROM_PTR(&mp_fat_vfs_type) },
};

STATIC MP_DEFINE_CONST_DICT(storage_module_globals, storage_module_globals_table);

const mp_obj_module_t storage_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&storage_module_globals,
};
