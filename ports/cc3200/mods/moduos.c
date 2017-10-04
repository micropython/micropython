/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>
#include <string.h>

#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "lib/timeutils/timeutils.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "genhdr/mpversion.h"
#include "moduos.h"
#include "sflash_diskio.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "random.h"
#include "mpexception.h"
#include "version.h"
#include "pybsd.h"
#include "pybuart.h"

/// \module os - basic "operating system" services
///
/// The `os` module contains functions for filesystem access and `urandom`.
///
/// The filesystem has `/` as the root directory, and the available physical
/// drives are accessible from here.  They are currently:
///
///     /flash      -- the serial flash filesystem
///
/// On boot up, the current directory is `/flash`.

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC os_term_dup_obj_t os_term_dup_obj;

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/

void osmount_unmount_all (void) {
    //TODO
    /*
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mount_obj_list).len; i++) {
        os_fs_mount_t *mount_obj = ((os_fs_mount_t *)(MP_STATE_PORT(mount_obj_list).items[i]));
        unmount(mount_obj);
    }
    */
}

/******************************************************************************/
// MicroPython bindings
//

STATIC const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_release_obj, WIPY_SW_VERSION_NUMBER);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);
STATIC MP_DEFINE_ATTRTUPLE(
    os_uname_info_obj,
    os_uname_info_fields,
    5,
    (mp_obj_t)&os_uname_info_sysname_obj,
    (mp_obj_t)&os_uname_info_nodename_obj,
    (mp_obj_t)&os_uname_info_release_obj,
    (mp_obj_t)&os_uname_info_version_obj,
    (mp_obj_t)&os_uname_info_machine_obj
);

STATIC mp_obj_t os_uname(void) {
    return (mp_obj_t)&os_uname_info_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_uname_obj, os_uname);

STATIC mp_obj_t os_sync(void) {
    sflash_disk_flush();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_sync_obj, os_sync);

STATIC mp_obj_t os_urandom(mp_obj_t num) {
    mp_int_t n = mp_obj_get_int(num);
    vstr_t vstr;
    vstr_init_len(&vstr, n);
    for (int i = 0; i < n; i++) {
        vstr.buf[i] = rng_get();
    }
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_urandom_obj, os_urandom);

STATIC mp_obj_t os_dupterm(uint n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        if (MP_STATE_PORT(os_term_dup_obj) == MP_OBJ_NULL) {
            return mp_const_none;
        } else {
            return MP_STATE_PORT(os_term_dup_obj)->stream_o;
        }
    } else {
        mp_obj_t stream_o = args[0];
        if (stream_o == mp_const_none) {
            MP_STATE_PORT(os_term_dup_obj) = MP_OBJ_NULL;
        } else {
            if (!MP_OBJ_IS_TYPE(stream_o, &pyb_uart_type)) {
                // must be a stream-like object providing at least read and write methods
                mp_load_method(stream_o, MP_QSTR_read, os_term_dup_obj.read);
                mp_load_method(stream_o, MP_QSTR_write, os_term_dup_obj.write);
            }
            os_term_dup_obj.stream_o = stream_o;
            MP_STATE_PORT(os_term_dup_obj) = &os_term_dup_obj;
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(os_dupterm_obj, 0, 1, os_dupterm);

STATIC const mp_rom_map_elem_t os_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_ROM_QSTR(MP_QSTR_uos) },

    { MP_ROM_QSTR(MP_QSTR_uname),           MP_ROM_PTR(&os_uname_obj) },

    { MP_ROM_QSTR(MP_QSTR_chdir),           MP_ROM_PTR(&mp_vfs_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd),          MP_ROM_PTR(&mp_vfs_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir),        MP_ROM_PTR(&mp_vfs_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_listdir),         MP_ROM_PTR(&mp_vfs_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir),           MP_ROM_PTR(&mp_vfs_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename),          MP_ROM_PTR(&mp_vfs_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove),          MP_ROM_PTR(&mp_vfs_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir),           MP_ROM_PTR(&mp_vfs_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat),            MP_ROM_PTR(&mp_vfs_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlink),          MP_ROM_PTR(&mp_vfs_remove_obj) },     // unlink aliases to remove

    { MP_ROM_QSTR(MP_QSTR_sync),            MP_ROM_PTR(&os_sync_obj) },
    { MP_ROM_QSTR(MP_QSTR_urandom),         MP_ROM_PTR(&os_urandom_obj) },

    // MicroPython additions
    // removed: mkfs
    // renamed: unmount -> umount
    { MP_ROM_QSTR(MP_QSTR_mount),           MP_ROM_PTR(&mp_vfs_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount),          MP_ROM_PTR(&mp_vfs_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_VfsFat),          MP_ROM_PTR(&mp_fat_vfs_type) },
    { MP_ROM_QSTR(MP_QSTR_dupterm),         MP_ROM_PTR(&os_dupterm_obj) },
};

STATIC MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t mp_module_uos = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&os_module_globals,
};
