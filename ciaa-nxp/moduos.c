/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/mpstate.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "genhdr/mpversion.h"
#include "memzip_extras.h"
#if 0
#include "timeutils.h"
#include "rng.h"
#include "file.h"
#include "sdcard.h"
#include "fsusermount.h"
#include "portmodules.h"
#endif

/// \module os - basic "operating system" services
///
/// The `os` module contains functions for filesystem access and `urandom`.
///
/// The filesystem has `/` as the root directory, and the available physical
/// drives are accessible from here.  They are currently:
///
///     /flash      -- the internal flash filesystem
///     /sd         -- the SD card (if it exists)
///
/// On boot up, the current directory is `/flash` if no SD card is inserted,
/// otherwise it is `/sd`.

STATIC const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, "pyboard");
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, "pyboard");
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_release_obj, MICROPY_VERSION_STRING);
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

/// \function chdir(path)
/// Change current directory.
STATIC mp_obj_t os_chdir(mp_obj_t path_in) {
	// TODO Implement me
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Unimplemented"));
	
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_chdir_obj, os_chdir);

/// \function getcwd()
/// Get the current directory.
STATIC mp_obj_t os_getcwd(void) {
    char buf[] = "/";
	// TODO Implement me

    return mp_obj_new_str(buf, strlen(buf), false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_getcwd_obj, os_getcwd);

typedef struct {
	mp_obj_t list;
	bool is_str_type;
	const char *path;
} os_listdir_data_t;

STATIC int os_listdir_helper(const MEMZIP_FILE_HDR *h, const char *name, void *user) {
	os_listdir_data_t *d = (os_listdir_data_t*) user;
	mp_obj_t entry_o;
	if (d->is_str_type) {
		entry_o = mp_obj_new_str(name, h->filename_len, false);
	} else {
		entry_o = mp_obj_new_bytes((const byte*)name, h->filename_len);
	}
	// add the entry to the list
	mp_obj_list_append(d->list, entry_o);

	return 1;
}

/// \function listdir([dir])
/// With no argument, list the current directory.  Otherwise list the given directory.
STATIC mp_obj_t os_listdir(mp_uint_t n_args, const mp_obj_t *args) {
	os_listdir_data_t d;
    if (n_args == 1) {
        if (mp_obj_get_type(args[0]) == &mp_type_bytes) {
            d.is_str_type = false;
        }
        d.path = mp_obj_str_get_str(args[0]);
    } else {
        d.path = "";
    }
    d.list = mp_obj_new_list(0, NULL);
	memzip_iterate(os_listdir_helper, &d);
    return d.list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(os_listdir_obj, 0, 1, os_listdir);

/// \function mkdir(path)
/// Create a new directory.
STATIC mp_obj_t os_mkdir(mp_obj_t path_o) {
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Read Only Filesystem"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_mkdir_obj, os_mkdir);

/// \function remove(path)
/// Remove a file.
STATIC mp_obj_t os_remove(mp_obj_t path_o) {
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Read Only Filesystem"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_remove_obj, os_remove);

/// \function rename(old_path, new_path)
/// Rename a file
STATIC mp_obj_t os_rename(mp_obj_t path_in, mp_obj_t path_out) {
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Read Only Filesystem"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(os_rename_obj, os_rename);

/// \function rmdir(path)
/// Remove a directory.
STATIC mp_obj_t os_rmdir(mp_obj_t path_o) {
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Read Only Filesystem"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_rmdir_obj, os_rmdir);

/// \function stat(path)
/// Get the status of a file or directory.
STATIC mp_obj_t os_stat(mp_obj_t path_in) {
	// TODO Implement me
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Unimplemented"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_stat_obj, os_stat);

/// \function sync()
/// Sync all filesystems.
STATIC mp_obj_t os_sync(void) {
	// FIXME Add functionality
	//nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Unimplemented"));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(mod_os_sync_obj, os_sync);

#if MICROPY_HW_ENABLE_RNG
/// \function urandom(n)
/// Return a bytes object with n random bytes, generated by the hardware
/// random number generator.
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
#endif

STATIC const mp_map_elem_t os_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_uos) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_uname), (mp_obj_t)&os_uname_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_chdir), (mp_obj_t)&os_chdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getcwd), (mp_obj_t)&os_getcwd_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listdir), (mp_obj_t)&os_listdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mkdir), (mp_obj_t)&os_mkdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove), (mp_obj_t)&os_remove_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rename),(mp_obj_t)&os_rename_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_rmdir), (mp_obj_t)&os_rmdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stat), (mp_obj_t)&os_stat_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unlink), (mp_obj_t)&os_remove_obj }, // unlink aliases to remove

    { MP_OBJ_NEW_QSTR(MP_QSTR_sync), (mp_obj_t)&mod_os_sync_obj },

    /// \constant sep - separation character used in paths
    { MP_OBJ_NEW_QSTR(MP_QSTR_sep), MP_OBJ_NEW_QSTR(MP_QSTR__slash_) },

#if MICROPY_HW_ENABLE_RNG
    { MP_OBJ_NEW_QSTR(MP_QSTR_urandom), (mp_obj_t)&os_urandom_obj },
#endif
};

STATIC MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t mp_module_uos = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uos,
    .globals = (mp_obj_dict_t*)&os_module_globals,
};
