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

#include "py/mpstate.h"
#include "py/nlr.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "genhdr/mpversion.h"
#include "moduos.h"
#include "diskio.h"
#include "sflash_diskio.h"
#include "extmod/vfs_fat_file.h"
#include "random.h"
#include "mpexception.h"
#include "version.h"
#include "timeutils.h"
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
STATIC uint32_t os_num_mounted_devices;
STATIC os_term_dup_obj_t os_term_dup_obj;

/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
STATIC void unmount (os_fs_mount_t *mount_obj);
STATIC bool path_equal(const char *path, const char *path_canonical);
STATIC void append_dir_item (mp_obj_t dirlist, const char *item, bool string);
STATIC void mount (mp_obj_t device, const char *path, uint pathlen, bool readonly);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/

void moduos_init0 (void) {
    // initialize the mount objects list
    mp_obj_list_init(&MP_STATE_PORT(mount_obj_list), 0);
    os_num_mounted_devices = 0;
}

os_fs_mount_t *osmount_find_by_path (const char *path) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mount_obj_list).len; i++) {
        os_fs_mount_t *mount_obj = ((os_fs_mount_t *)(MP_STATE_PORT(mount_obj_list).items[i]));
        if (!strcmp(path, mount_obj->path)) {
            return mount_obj;
        }
    }
    return NULL;
}

os_fs_mount_t *osmount_find_by_volume (uint8_t vol) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mount_obj_list).len; i++) {
        os_fs_mount_t *mount_obj = ((os_fs_mount_t *)(MP_STATE_PORT(mount_obj_list).items[i]));
        if (vol == mount_obj->vol) {
            return mount_obj;
        }
    }
    return NULL;
}

os_fs_mount_t *osmount_find_by_device (mp_obj_t device) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mount_obj_list).len; i++) {
        os_fs_mount_t *mount_obj = ((os_fs_mount_t *)(MP_STATE_PORT(mount_obj_list).items[i]));
        if (device == mount_obj->device) {
            return mount_obj;
        }
    }
    return NULL;
}

void osmount_unmount_all (void) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mount_obj_list).len; i++) {
        os_fs_mount_t *mount_obj = ((os_fs_mount_t *)(MP_STATE_PORT(mount_obj_list).items[i]));
        unmount(mount_obj);
    }
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/

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

STATIC void append_dir_item (mp_obj_t dirlist, const char *item, bool string) {
    // make a string object for this entry
    mp_obj_t entry_o;
    if (string) {
        entry_o = mp_obj_new_str(item, strlen(item), false);
    } else {
        entry_o = mp_obj_new_bytes((const byte*)item, strlen(item));
    }

    // add the entry to the list
    mp_obj_list_append(dirlist, entry_o);
}

STATIC void mount (mp_obj_t device, const char *path, uint pathlen, bool readonly) {
    // is the mount point already in use?
    FILINFO fno;
#if _USE_LFN
    fno.lfname = NULL;
    fno.lfsize = 0;
#endif
    // cannot mount twice or on existing paths
    if (f_stat(path, &fno) == FR_OK || osmount_find_by_device(device)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

    // create a new object
    os_fs_mount_t *self = m_new_obj(os_fs_mount_t);
    self->device = device;
    self->path = path;
    self->pathlen = pathlen;
    self->vol = os_num_mounted_devices + 1;     // '/flash' is volume 0

    if (device == (mp_obj_t)&pybsd_obj) {
        // need to make it different to NULL, otherwise it's read only by default
        self->writeblocks[0] = mp_const_none;
        self->sync[0] = MP_OBJ_NULL;    // no need to sync the SD card
        self->count[0] = MP_OBJ_NULL;
    } else {
        // load block protocol methods
        mp_load_method(device, MP_QSTR_readblocks, self->readblocks);
        mp_load_method_maybe(device, MP_QSTR_writeblocks, self->writeblocks);
        mp_load_method_maybe(device, MP_QSTR_sync, self->sync);
        mp_load_method(device, MP_QSTR_count, self->count);
    }

    // Read-only device indicated by writeblocks[0] == MP_OBJ_NULL.
    // User can specify read-only device by:
    //  1. readonly=True keyword argument
    //  2. nonexistent writeblocks method (then writeblocks[0] == MP_OBJ_NULL already)
    if (readonly) {
        self->writeblocks[0] = MP_OBJ_NULL;
    }

    // we need to add it before doing the actual mount, so that the volume can be found
    mp_obj_list_append(&MP_STATE_PORT(mount_obj_list), self);

    // actually mount it
    if (f_mount(&self->fatfs, self->path, 1) != FR_OK) {
        // remove it and raise
        mp_obj_list_remove(&MP_STATE_PORT(mount_obj_list), self);
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }

    // mount succeeded, increment the count
    os_num_mounted_devices++;
}

STATIC void unmount (os_fs_mount_t *mount_obj) {
    // remove it from the list and then call FatFs
    f_mount (NULL, mount_obj->path, 1);
    mp_obj_list_remove(&MP_STATE_PORT(mount_obj_list), mount_obj);
    os_num_mounted_devices--;
}

/******************************************************************************/
// Micro Python bindings
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

/// \function chdir(path)
/// Change current directory.
STATIC mp_obj_t os_chdir(mp_obj_t path_in) {
    const char *path;
    path = mp_obj_str_get_str(path_in);

    FRESULT res = f_chdrive(path);

    if (res == FR_OK) {
        res = f_chdir(path);
    }

    if (res != FR_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_chdir_obj, os_chdir);

STATIC mp_obj_t os_getcwd(void) {
    char buf[MICROPY_ALLOC_PATH_MAX + 1];
    FRESULT res = f_getcwd(buf, sizeof buf);
    if (res != FR_OK) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(fresult_to_errno_table[res])));
    }
    return mp_obj_new_str(buf, strlen(buf), false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(os_getcwd_obj, os_getcwd);

STATIC mp_obj_t os_listdir(mp_uint_t n_args, const mp_obj_t *args) {
    bool is_str_type = true;
    const char *path;
    mp_obj_t dir_list = mp_obj_new_list(0, NULL);

    if (n_args == 1) {
        if (mp_obj_get_type(args[0]) == &mp_type_bytes) {
            is_str_type = false;
        }
        path = mp_obj_str_get_str(args[0]);
    } else {
        path = "";
    }

    // "hack" to list the root directory
    if (path[0] == '/' && path[1] == '\0') {
        // add 'flash' to the list
        append_dir_item (dir_list, "flash", is_str_type);
        for (mp_uint_t i = 0; i < MP_STATE_PORT(mount_obj_list).len; i++) {
            os_fs_mount_t *mount_obj = ((os_fs_mount_t *)(MP_STATE_PORT(mount_obj_list).items[i]));
            append_dir_item (dir_list, &mount_obj->path[1], is_str_type);
        }
    } else {
        FRESULT res;
        DIR dir;
        FILINFO fno;
    #if _USE_LFN
        char lfn_buf[_MAX_LFN + 1];
        fno.lfname = lfn_buf;
        fno.lfsize = sizeof(lfn_buf);
    #endif

        res = f_opendir(&dir, path);                       /* Open the directory */
        if (res != FR_OK) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
        }

        for ( ; ; ) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.' && fno.fname[1] == 0) continue;             /* Ignore . entry */
            if (fno.fname[0] == '.' && fno.fname[1] == '.' && fno.fname[2] == 0) continue;             /* Ignore .. entry */

        #if _USE_LFN
            char *fn = *fno.lfname ? fno.lfname : fno.fname;
        #else
            char *fn = fno.fname;
        #endif

            // add the entry to the list
            append_dir_item (dir_list, fn, is_str_type);
        }
        f_closedir(&dir);
    }

    return dir_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(os_listdir_obj, 0, 1, os_listdir);

STATIC mp_obj_t os_mkdir(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);
    FRESULT res = f_mkdir(path);
    switch (res) {
        case FR_OK:
            return mp_const_none;
        case FR_EXIST:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
            break;
        default:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_mkdir_obj, os_mkdir);

STATIC mp_obj_t os_rename(mp_obj_t path_in, mp_obj_t path_out) {
    const char *old_path = mp_obj_str_get_str(path_in);
    const char *new_path = mp_obj_str_get_str(path_out);
    FRESULT res = f_rename(old_path, new_path);
    switch (res) {
        case FR_OK:
            return mp_const_none;
        default:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(os_rename_obj, os_rename);

STATIC mp_obj_t os_remove(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);
    FRESULT res = f_unlink(path);
    switch (res) {
        case FR_OK:
            return mp_const_none;
        default:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_remove_obj, os_remove);

STATIC mp_obj_t os_stat(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    bool isbuilt_in = false;
    FILINFO fno;
    FRESULT res;
#if _USE_LFN
    fno.lfname = NULL;
    fno.lfsize = 0;
#endif

    // check on the user mounted devices
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mount_obj_list).len; i++) {
        os_fs_mount_t *mount_obj = ((os_fs_mount_t *)(MP_STATE_PORT(mount_obj_list).items[i]));
        if (path_equal(path, mount_obj->path)) {
            isbuilt_in = true;
            break;
        }
    }

    if (path_equal(path, "/") || path_equal(path, "/flash") || isbuilt_in) {
        // stat built-in directory
        fno.fsize = 0;
        fno.fdate = 0;
        fno.ftime = 0;
        fno.fattrib = AM_DIR;
    } else if ((res = f_stat(path, &fno)) != FR_OK) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(fresult_to_errno_table[res])));
    }

    mp_obj_tuple_t *t = mp_obj_new_tuple(10, NULL);
    mp_int_t mode = 0;
    if (fno.fattrib & AM_DIR) {
        mode |= 0x4000; // stat.S_IFDIR
    } else {
        mode |= 0x8000; // stat.S_IFREG
    }
    mp_int_t seconds = timeutils_seconds_since_2000(
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
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_stat_obj, os_stat);

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

STATIC mp_obj_t os_mount(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t mount_args[] = {
        { MP_QSTR_readonly,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    mp_obj_t device = pos_args[0];
    mp_obj_t mount_point = pos_args[1];
    mp_arg_val_t args[MP_ARRAY_SIZE(mount_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(mount_args), mount_args, args);

    // get the mount point
    mp_uint_t pathlen;
    const char *path_in = mp_obj_str_get_data(mount_point, &pathlen);
    if (pathlen == 0) {
        goto invalid_args;
    }

    char *path = m_new(char, pathlen + 1);
    memcpy(path, path_in, pathlen);
    path[pathlen] = '\0';

    // "remove" any extra slahes at the end
    while (path[(pathlen - 1)] == '/') {
        path[--pathlen] = '\0';
    }

    // is the mount point valid?
    if (pathlen < 2 || path[0] !='/' || strchr(&path[1], '/')) {
        goto invalid_args;
    }

    // now mount it
    mount(device, path, pathlen, args[0].u_bool);

    return mp_const_none;

invalid_args:
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_value_invalid_arguments));
}
MP_DEFINE_CONST_FUN_OBJ_KW(os_mount_obj, 2, os_mount);

STATIC mp_obj_t os_unmount(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);

    // '/flash' cannot be unmounted, also not the current working directory
    if (path_equal(path, "/flash")) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

    // now unmount it
    os_fs_mount_t *mount_obj;
    if ((mount_obj = osmount_find_by_path(path))) {
        unmount (mount_obj);
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_unmount_obj, os_unmount);

STATIC mp_obj_t os_mkfs(mp_obj_t device) {
    const char *path = "/__mkfs__mnt__";
    os_fs_mount_t *mount_obj = NULL;
    bool unmt = false;
    FRESULT res;

    if (MP_OBJ_IS_STR_OR_BYTES(device)) {
        path = mp_obj_str_get_str(device);
        // otherwise the relative path check will pass...
        if (path[0] != '/') {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_value_invalid_arguments));
        }
    } else {
        // mount it briefly
        mount(device, path, strlen(path), false);
        unmt = true;
    }

    byte sfd = 0;
    if (!memcmp(path, "/flash", strlen("/flash"))) {
        sfd = 1;
    } else if ((mount_obj = osmount_find_by_path(path))) {
        if (mount_obj->device != (mp_obj_t)&pybsd_obj &&
            mp_obj_get_int(mp_call_method_n_kw(0, 0, mount_obj->count)) < 2048) {
            sfd = 1;
        }
    }

    // now format the device
    res = f_mkfs(path, sfd, 0);

    if (unmt && mount_obj) {
        unmount (mount_obj);
    }

    if (res != FR_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(os_mkfs_obj, os_mkfs);

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

STATIC const mp_map_elem_t os_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_uos) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_uname),           (mp_obj_t)&os_uname_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_chdir),           (mp_obj_t)&os_chdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getcwd),          (mp_obj_t)&os_getcwd_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listdir),         (mp_obj_t)&os_listdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mkdir),           (mp_obj_t)&os_mkdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rename),          (mp_obj_t)&os_rename_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove),          (mp_obj_t)&os_remove_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rmdir),           (mp_obj_t)&os_remove_obj },     // rmdir aliases to remove
    { MP_OBJ_NEW_QSTR(MP_QSTR_stat),            (mp_obj_t)&os_stat_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unlink),          (mp_obj_t)&os_remove_obj },     // unlink aliases to remove
    { MP_OBJ_NEW_QSTR(MP_QSTR_sync),            (mp_obj_t)&os_sync_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_urandom),         (mp_obj_t)&os_urandom_obj },

    // MicroPython additions
    { MP_OBJ_NEW_QSTR(MP_QSTR_mount),           (mp_obj_t)&os_mount_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unmount),         (mp_obj_t)&os_unmount_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mkfs),            (mp_obj_t)&os_mkfs_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_dupterm),         (mp_obj_t)&os_dupterm_obj },

    /// \constant sep - separation character used in paths
    { MP_OBJ_NEW_QSTR(MP_QSTR_sep),             MP_OBJ_NEW_QSTR(MP_QSTR__slash_) },
};

STATIC MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t mp_module_uos = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uos,
    .globals = (mp_obj_dict_t*)&os_module_globals,
};
