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

#include <string.h>
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "lib/fatfs/ff.h"
#include "lib/fatfs/diskio.h"
#include "extmod/vfs_fat_file.h"
#include "extmod/fsusermount.h"
#include "timeutils.h"

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

STATIC mp_obj_t fat_vfs_listdir_func(size_t n_args, const mp_obj_t *args) {
    bool is_str_type = true;
    const char *path;
    if (n_args == 2) {
        if (mp_obj_get_type(args[1]) == &mp_type_bytes) {
            is_str_type = false;
        }
        path = mp_obj_str_get_str(args[1]);
    } else {
        path = "";
    }

    return fat_vfs_listdir(path, is_str_type);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fat_vfs_listdir_obj, 1, 2, fat_vfs_listdir_func);

STATIC mp_obj_t fat_vfs_remove_internal(mp_obj_t path_in, mp_int_t attr) {
    const char *path = mp_obj_str_get_str(path_in);

    FILINFO fno;
#if _USE_LFN
    fno.lfname = NULL;
    fno.lfsize = 0;
#endif
    FRESULT res = f_stat(path, &fno);

    if (res != FR_OK) {
        mp_raise_OSError(fresult_to_errno_table[res]);
    }

    // check if path is a file or directory
    if ((fno.fattrib & AM_DIR) == attr) {
        res = f_unlink(path);

        if (res != FR_OK) {
            mp_raise_OSError(fresult_to_errno_table[res]);
        }
        return mp_const_none;
    } else {
        mp_raise_OSError(attr ? MP_ENOTDIR : MP_EISDIR);
    }
}

STATIC mp_obj_t fat_vfs_remove(mp_obj_t vfs_in, mp_obj_t path_in) {
    (void)vfs_in;
    return fat_vfs_remove_internal(path_in, 0); // 0 == file attribute
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_remove_obj, fat_vfs_remove);

STATIC mp_obj_t fat_vfs_rmdir(mp_obj_t vfs_in, mp_obj_t path_in) {
    (void) vfs_in;
    return fat_vfs_remove_internal(path_in, AM_DIR);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_rmdir_obj, fat_vfs_rmdir);

STATIC mp_obj_t fat_vfs_rename(mp_obj_t vfs_in, mp_obj_t path_in, mp_obj_t path_out) {
    (void)vfs_in;
    const char *old_path = mp_obj_str_get_str(path_in);
    const char *new_path = mp_obj_str_get_str(path_out);
    FRESULT res = f_rename(old_path, new_path);
    if (res == FR_EXIST) {
        // if new_path exists then try removing it (but only if it's a file)
        fat_vfs_remove_internal(path_out, 0); // 0 == file attribute
        // try to rename again
        res = f_rename(old_path, new_path);
    }
    if (res == FR_OK) {
        return mp_const_none;
    } else {
        mp_raise_OSError(fresult_to_errno_table[res]);
    }

}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(fat_vfs_rename_obj, fat_vfs_rename);

STATIC mp_obj_t fat_vfs_mkdir(mp_obj_t vfs_in, mp_obj_t path_o) {
    (void)vfs_in;
    const char *path = mp_obj_str_get_str(path_o);
    FRESULT res = f_mkdir(path);
    if (res == FR_OK) {
        return mp_const_none;
    } else {
        mp_raise_OSError(fresult_to_errno_table[res]);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_mkdir_obj, fat_vfs_mkdir);

/// Change current directory.
STATIC mp_obj_t fat_vfs_chdir(mp_obj_t vfs_in, mp_obj_t path_in) {
    (void)vfs_in;
    const char *path;
    path = mp_obj_str_get_str(path_in);

    FRESULT res = f_chdrive(path);

    if (res == FR_OK) {
        res = f_chdir(path);
    }

    if (res != FR_OK) {
        mp_raise_OSError(fresult_to_errno_table[res]);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_chdir_obj, fat_vfs_chdir);

/// Get the current directory.
STATIC mp_obj_t fat_vfs_getcwd(mp_obj_t vfs_in) {
    (void)vfs_in;
    char buf[MICROPY_ALLOC_PATH_MAX + 1];
    FRESULT res = f_getcwd(buf, sizeof buf);

    if (res != FR_OK) {
        mp_raise_OSError(fresult_to_errno_table[res]);
    }

    return mp_obj_new_str(buf, strlen(buf), false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fat_vfs_getcwd_obj, fat_vfs_getcwd);

// Checks for path equality, ignoring trailing slashes:
//   path_equal(/, /) -> true
// second argument must be in canonical form (meaning no trailing slash, unless it's just /)
STATIC bool path_equal(const char *path, const char *path_canonical) {
    while (*path_canonical != '\0' && *path == *path_canonical) {
        ++path;
        ++path_canonical;
    }
    if (*path_canonical != '\0') {
        return false;
    }
    while (*path == '/') {
        ++path;
    }
    return *path == '\0';
}

/// \function stat(path)
/// Get the status of a file or directory.
STATIC mp_obj_t fat_vfs_stat(mp_obj_t vfs_in, mp_obj_t path_in) {
    (void)vfs_in;
    const char *path = mp_obj_str_get_str(path_in);

    FILINFO fno;
#if _USE_LFN
    fno.lfname = NULL;
    fno.lfsize = 0;
#endif
    FRESULT res;

    if (path_equal(path, "/")) {
        // stat root directory
        fno.fsize = 0;
        fno.fdate = 0x2821; // Jan 1, 2000
        fno.ftime = 0;
        fno.fattrib = AM_DIR;
    } else {
        res = FR_NO_PATH;
        for (size_t i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(fs_user_mount)); ++i) {
            fs_user_mount_t *vfs = MP_STATE_PORT(fs_user_mount)[i];
            if (vfs != NULL && path_equal(path, vfs->str)) {
                // stat mounted device directory
                fno.fsize = 0;
                fno.fdate = 0x2821; // Jan 1, 2000
                fno.ftime = 0;
                fno.fattrib = AM_DIR;
                res = FR_OK;
            }
        }
        if (res == FR_NO_PATH) {
            // stat normal file
            res = f_stat(path, &fno);
        }
        if (res != FR_OK) {
            mp_raise_OSError(fresult_to_errno_table[res]);
        }
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
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
    t->items[0] = MP_OBJ_NEW_SMALL_INT(mode); // st_mode
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0); // st_ino
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // st_dev
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0); // st_nlink
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0); // st_uid
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // st_gid
    t->items[6] = MP_OBJ_NEW_SMALL_INT(fno.fsize); // st_size
    t->items[7] = MP_OBJ_NEW_SMALL_INT(seconds); // st_atime
    t->items[8] = MP_OBJ_NEW_SMALL_INT(seconds); // st_mtime
    t->items[9] = MP_OBJ_NEW_SMALL_INT(seconds); // st_ctime

    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_stat_obj, fat_vfs_stat);

// Get the status of a VFS.
STATIC mp_obj_t fat_vfs_statvfs(mp_obj_t vfs_in, mp_obj_t path_in) {
    (void)vfs_in;
    const char *path = mp_obj_str_get_str(path_in);

    FATFS *fatfs;
    DWORD nclst;
    FRESULT res = f_getfree(path, &nclst, &fatfs);
    if (FR_OK != res) {
        mp_raise_OSError(fresult_to_errno_table[res]);
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));

    t->items[0] = MP_OBJ_NEW_SMALL_INT(fatfs->csize * fatfs->ssize); // f_bsize
    t->items[1] = t->items[0]; // f_frsize
    t->items[2] = MP_OBJ_NEW_SMALL_INT((fatfs->n_fatent - 2) * fatfs->csize); // f_blocks
    t->items[3] = MP_OBJ_NEW_SMALL_INT(nclst); // f_bfree
    t->items[4] = t->items[3]; // f_bavail
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // f_files
    t->items[6] = MP_OBJ_NEW_SMALL_INT(0); // f_ffree
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0); // f_favail
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // f_flags
    t->items[9] = MP_OBJ_NEW_SMALL_INT(_MAX_LFN); // f_namemax

    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_statvfs_obj, fat_vfs_statvfs);

// Unmount the filesystem
STATIC mp_obj_t fat_vfs_umount(mp_obj_t vfs_in) {
    fatfs_umount(((fs_user_mount_t *)vfs_in)->readblocks[1]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fat_vfs_umount_obj, fat_vfs_umount);

STATIC const mp_rom_map_elem_t fat_vfs_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mkfs), MP_ROM_PTR(&fat_vfs_mkfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&fat_vfs_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&fat_vfs_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&fat_vfs_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&fat_vfs_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&fat_vfs_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&fat_vfs_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&fat_vfs_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&fat_vfs_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&fat_vfs_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&fat_vfs_statvfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&fat_vfs_umount_obj) },
};
STATIC MP_DEFINE_CONST_DICT(fat_vfs_locals_dict, fat_vfs_locals_dict_table);

const mp_obj_type_t mp_fat_vfs_type = {
    { &mp_type_type },
    .name = MP_QSTR_VfsFat,
    .make_new = fat_vfs_make_new,
    .locals_dict = (mp_obj_dict_t*)&fat_vfs_locals_dict,
};

#endif // MICROPY_VFS_FAT
