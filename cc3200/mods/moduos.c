/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include "std.h"

#include "py/mpconfig.h"
#include "py/nlr.h"
#include "py/obj.h"
#include "py/objtuple.h"
#include "ff.h"
#include "diskio.h"
#include "sflash_diskio.h"
#include "file.h"
#include "modutime.h"
#include "random.h"
#include "sd_diskio.h"

/// \module os - basic "operating system" services
///
/// The `os` module contains functions for filesystem access and `urandom`.
///
/// The filesystem has `/` as the root directory, and the available physical
/// drives are accessible from here.  They are currently:
///
///     /SFLASH     -- the serial flash filesystem
///     /SD         -- the SD card (if it exists)
///
/// On boot up, the current directory is `/SFLASH` if no SD card is inserted,
/// otherwise it is `/SD`.

STATIC bool sd_in_root(void) {
#if MICROPY_HW_HAS_SDCARD
    return sd_disk_ready();
#else
    return false;
#endif
}

/// \function chdir(path)
/// Change current directory.
STATIC mp_obj_t os_chdir(mp_obj_t path_in) {
    const char *path;
    path = mp_obj_str_get_str(path_in);

    FRESULT res = f_chdrive(path);

    if (res == FR_OK) {
        res = f_chdir(path);
    }
    // TODO: Warn if too many open files...
    if (res != FR_OK) {
        // TODO should be mp_type_FileNotFoundError
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "No such file or directory: '%s'", path));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_chdir_obj, os_chdir);

/// \function getcwd()
/// Get the current directory.
STATIC mp_obj_t os_getcwd(void) {
    char buf[MICROPY_ALLOC_PATH_MAX + 1];
    FRESULT res = f_getcwd(buf, sizeof buf);

    if (res != FR_OK) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(fresult_to_errno_table[res])));
    }

    return mp_obj_new_str(buf, strlen(buf), false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_getcwd_obj, os_getcwd);

/// \function listdir([dir])
/// With no argument, list the current directory.  Otherwise list the given directory.
STATIC mp_obj_t os_listdir(mp_uint_t n_args, const mp_obj_t *args) {
    bool is_str_type = true;
    const char *path;
    if (n_args == 1) {
        if (mp_obj_get_type(args[0]) == &mp_type_bytes) {
            is_str_type = false;
        }
        path = mp_obj_str_get_str(args[0]);
    } else {
        path = "";
    }

    // "hack" to list root directory
    if (path[0] == '/' && path[1] == '\0') {
        mp_obj_t dir_list = mp_obj_new_list(0, NULL);
        mp_obj_list_append(dir_list, MP_OBJ_NEW_QSTR(MP_QSTR_SFLASH));
        if (sd_in_root()) {
            mp_obj_list_append(dir_list, MP_OBJ_NEW_QSTR(MP_QSTR_SD));
        }
        return dir_list;
    }

    FRESULT res;
    FILINFO fno;
    DIR dir;

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res != FR_OK) {
        // TODO should be mp_type_FileNotFoundError
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "No such file or directory: '%s'", path));
    }

    mp_obj_t dir_list = mp_obj_new_list(0, NULL);

    for (;;) {
        res = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fname[0] == '.' && fno.fname[1] == 0) continue;             /* Ignore . entry */
        if (fno.fname[0] == '.' && fno.fname[1] == '.' && fno.fname[2] == 0) continue;             /* Ignore .. entry */

        char *fn = fno.fname;

        // make a string object for this entry
        mp_obj_t entry_o;
        if (is_str_type) {
            entry_o = mp_obj_new_str(fn, strlen(fn), false);
        } else {
            entry_o = mp_obj_new_bytes((const byte*)fn, strlen(fn));
        }

        // add the entry to the list
        mp_obj_list_append(dir_list, entry_o);
    }

    f_closedir(&dir);

    return dir_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(os_listdir_obj, 0, 1, os_listdir);

/// \function mkdir(path)
/// Create a new directory.
STATIC mp_obj_t os_mkdir(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);
    FRESULT res = f_mkdir(path);
    switch (res) {
        case FR_OK:
            return mp_const_none;
        case FR_EXIST:
            // TODO should be FileExistsError
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "File exists: '%s'", path));
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error creating directory '%s'", path));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_mkdir_obj, os_mkdir);

/// \function remove(path)
/// Remove a file.
STATIC mp_obj_t os_remove(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);
    // TODO check that path is actually a file before trying to unlink it
    FRESULT res = f_unlink(path);
    switch (res) {
        case FR_OK:
            return mp_const_none;
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error removing file '%s'", path));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_remove_obj, os_remove);

/// \function rmdir(path)
/// Remove a directory.
STATIC mp_obj_t os_rmdir(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);
    // TODO check that path is actually a directory before trying to unlink it
    FRESULT res = f_unlink(path);
    switch (res) {
        case FR_OK:
            return mp_const_none;
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error removing directory '%s'", path));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_rmdir_obj, os_rmdir);

// Checks for path equality, ignoring trailing slashes:
//   path_equal(/, /) -> true
//   path_equal(/flash//, /flash) -> true
// second argument must be in canonical form (meaning no trailing slash, unless it's just /)
STATIC bool path_equal(const char *path, const char *path_canonical) {
    for (; *path_canonical != '\0' && *path == *path_canonical; ++path, ++path_canonical) {
    }
    if (*path_canonical != '\0') {
        return false;
    }
    for (; *path == '/'; ++path) {
    }
    return *path == '\0';
}

/// \function stat(path)
/// Get the status of a file or directory.
STATIC mp_obj_t os_stat(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    stoupper((char *)path);

    FILINFO fno;
    FRESULT res;
    if (path_equal(path, "/") || path_equal(path, "/SFLASH") || path_equal(path, "/SD")) {
        // stat built-in directory
        if (path[1] == 'S' && !sd_in_root()) {
            // no /SD directory
            res = FR_NO_PATH;
            goto error;
        }
	fno.fsize = 0;
	fno.fdate = 0;
	fno.ftime = 0;
	fno.fattrib = AM_DIR;
    } else {
        res = f_stat(path, &fno);
        if (res != FR_OK) {
            goto error;
        }
    }

    mp_obj_tuple_t *t = mp_obj_new_tuple(10, NULL);
    mp_int_t mode = 0;
    if (fno.fattrib & AM_DIR) {
        mode |= 0x4000; // stat.S_IFDIR
    } else {
        mode |= 0x8000; // stat.S_IFREG
    }
    mp_int_t seconds = mod_time_seconds_since_2000(
        1980 + ((fno.fdate >> 9) & 0x7f),
        (fno.fdate >> 5) & 0x0f,
        fno.fdate & 0x1f,
        (fno.ftime >> 11) & 0x1f,
        (fno.ftime >> 5) & 0x3f,
        2 * (fno.ftime & 0x1f)
    );
    t->items[0] = mp_obj_new_int(mode);      // st_mode
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0);   // st_ino
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0);   // st_dev
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0);   // st_nlink
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0);   // st_uid
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0);   // st_gid
    t->items[6] = mp_obj_new_int(fno.fsize); // st_size
    t->items[7] = mp_obj_new_int(seconds);   // st_atime
    t->items[8] = t->items[7];               // st_mtime
    t->items[9] = t->items[7];               // st_ctime

    return t;

error:
    nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(fresult_to_errno_table[res])));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_stat_obj, os_stat);

/// \function sync()
/// Sync all filesystems.
STATIC mp_obj_t os_sync(void) {
    sflash_disk_flush();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_sync_obj, os_sync);

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

    { MP_OBJ_NEW_QSTR(MP_QSTR_chdir), (mp_obj_t)&os_chdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getcwd), (mp_obj_t)&os_getcwd_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listdir), (mp_obj_t)&os_listdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mkdir), (mp_obj_t)&os_mkdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove), (mp_obj_t)&os_remove_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rmdir), (mp_obj_t)&os_rmdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stat), (mp_obj_t)&os_stat_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unlink), (mp_obj_t)&os_remove_obj }, // unlink aliases to remove

    { MP_OBJ_NEW_QSTR(MP_QSTR_sync), (mp_obj_t)&os_sync_obj },

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
