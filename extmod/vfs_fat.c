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

#if !MICROPY_VFS
#error "with MICROPY_VFS_FAT enabled, must also enable MICROPY_VFS"
#endif

#include <string.h>
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"
#include "shared/timeutils/timeutils.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/translate/translate.h"

#if FF_MAX_SS == FF_MIN_SS
#define SECSIZE(fs) (FF_MIN_SS)
#else
#define SECSIZE(fs) ((fs)->ssize)
#endif

#define mp_obj_fat_vfs_t fs_user_mount_t

// Factoring this common call saves about 90 bytes.
STATIC NORETURN void mp_raise_OSError_fresult(FRESULT res) {
    mp_raise_OSError(fresult_to_errno_table[res]);
}

STATIC mp_import_stat_t fat_vfs_import_stat(void *vfs_in, const char *path) {
    fs_user_mount_t *vfs = vfs_in;
    FILINFO fno;
    assert(vfs != NULL);
    FRESULT res = f_stat(&vfs->fatfs, path, &fno);
    if (res == FR_OK) {
        if ((fno.fattrib & AM_DIR) != 0) {
            return MP_IMPORT_STAT_DIR;
        } else {
            return MP_IMPORT_STAT_FILE;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

STATIC mp_obj_t fat_vfs_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // create new object
    fs_user_mount_t *vfs = mp_obj_malloc(fs_user_mount_t, type);
    vfs->fatfs.drv = vfs;

    // Initialise underlying block device
    vfs->blockdev.flags = MP_BLOCKDEV_FLAG_FREE_OBJ;
    vfs->blockdev.block_size = FF_MIN_SS; // default, will be populated by call to MP_BLOCKDEV_IOCTL_BLOCK_SIZE
    mp_vfs_blockdev_init(&vfs->blockdev, args[0]);

    // mount the block device so the VFS methods can be used
    FRESULT res = f_mount(&vfs->fatfs);
    if (res == FR_NO_FILESYSTEM) {
        // don't error out if no filesystem, to let mkfs()/mount() create one if wanted
        vfs->blockdev.flags |= MP_BLOCKDEV_FLAG_NO_FILESYSTEM;
    } else if (res != FR_OK) {
        mp_raise_OSError_fresult(res);
    }

    return MP_OBJ_FROM_PTR(vfs);
}

STATIC void verify_fs_writable(fs_user_mount_t *vfs) {
    if (!filesystem_is_writable_by_python(vfs)) {
        mp_raise_OSError(MP_EROFS);
    }
}

#if FF_FS_REENTRANT
STATIC mp_obj_t fat_vfs_del(mp_obj_t self_in) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(self_in);
    // f_umount only needs to be called to release the sync object
    f_umount(&self->fatfs);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fat_vfs_del_obj, fat_vfs_del);
#endif

STATIC mp_obj_t fat_vfs_mkfs(mp_obj_t bdev_in) {
    // create new object
    fs_user_mount_t *vfs = MP_OBJ_TO_PTR(fat_vfs_make_new(&mp_fat_vfs_type, 1, 0, &bdev_in));

    // make the filesystem
    uint8_t working_buf[FF_MAX_SS];
    FRESULT res = f_mkfs(&vfs->fatfs, FM_FAT | FM_SFD, 0, working_buf, sizeof(working_buf));
    if (res == FR_MKFS_ABORTED) { // Probably doesn't support FAT16
        res = f_mkfs(&vfs->fatfs, FM_FAT32, 0, working_buf, sizeof(working_buf));
    }
    if (res != FR_OK) {
        mp_raise_OSError_fresult(res);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fat_vfs_mkfs_fun_obj, fat_vfs_mkfs);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(fat_vfs_mkfs_obj, MP_ROM_PTR(&fat_vfs_mkfs_fun_obj));

typedef struct _mp_vfs_fat_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    bool is_str;
    FF_DIR dir;
} mp_vfs_fat_ilistdir_it_t;

STATIC mp_obj_t mp_vfs_fat_ilistdir_it_iternext(mp_obj_t self_in) {
    mp_vfs_fat_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);

    for (;;) {
        FILINFO fno;
        FRESULT res = f_readdir(&self->dir, &fno);
        char *fn = fno.fname;
        if (res != FR_OK || fn[0] == 0) {
            // stop on error or end of dir
            break;
        }

        // Note that FatFS already filters . and .., so we don't need to

        // make 4-tuple with info about this entry
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(4, NULL));
        if (self->is_str) {
            t->items[0] = mp_obj_new_str(fn, strlen(fn));
        } else {
            t->items[0] = mp_obj_new_bytes((const byte *)fn, strlen(fn));
        }
        if (fno.fattrib & AM_DIR) {
            // dir
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR);
        } else {
            // file
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG);
        }
        t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // no inode number
        t->items[3] = mp_obj_new_int_from_uint(fno.fsize);

        return MP_OBJ_FROM_PTR(t);
    }

    // ignore error because we may be closing a second time
    f_closedir(&self->dir);

    return MP_OBJ_STOP_ITERATION;
}

STATIC mp_obj_t fat_vfs_ilistdir_func(size_t n_args, const mp_obj_t *args) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(args[0]);
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

    // Create a new iterator object to list the dir
    mp_vfs_fat_ilistdir_it_t *iter = mp_obj_malloc(mp_vfs_fat_ilistdir_it_t, &mp_type_polymorph_iter);
    iter->iternext = mp_vfs_fat_ilistdir_it_iternext;
    iter->is_str = is_str_type;
    FRESULT res = f_opendir(&self->fatfs, &iter->dir, path);
    if (res != FR_OK) {
        mp_raise_OSError_fresult(res);
    }

    return MP_OBJ_FROM_PTR(iter);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fat_vfs_ilistdir_obj, 1, 2, fat_vfs_ilistdir_func);

STATIC mp_obj_t fat_vfs_remove_internal(mp_obj_t vfs_in, mp_obj_t path_in, mp_int_t attr) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(vfs_in);
    verify_fs_writable(self);
    const char *path = mp_obj_str_get_str(path_in);

    FILINFO fno;
    FRESULT res = f_stat(&self->fatfs, path, &fno);

    if (res != FR_OK) {
        mp_raise_OSError_fresult(res);
    }

    // check if path is a file or directory
    if ((fno.fattrib & AM_DIR) == attr) {
        res = f_unlink(&self->fatfs, path);

        if (res != FR_OK) {
            mp_raise_OSError_fresult(res);
        }
        return mp_const_none;
    } else {
        mp_raise_OSError(attr ? MP_ENOTDIR : MP_EISDIR);
    }
}

STATIC mp_obj_t fat_vfs_remove(mp_obj_t vfs_in, mp_obj_t path_in) {
    return fat_vfs_remove_internal(vfs_in, path_in, 0); // 0 == file attribute
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_remove_obj, fat_vfs_remove);

STATIC mp_obj_t fat_vfs_rmdir(mp_obj_t vfs_in, mp_obj_t path_in) {
    return fat_vfs_remove_internal(vfs_in, path_in, AM_DIR);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_rmdir_obj, fat_vfs_rmdir);

STATIC mp_obj_t fat_vfs_rename(mp_obj_t vfs_in, mp_obj_t path_in, mp_obj_t path_out) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(vfs_in);
    verify_fs_writable(self);
    const char *old_path = mp_obj_str_get_str(path_in);
    const char *new_path = mp_obj_str_get_str(path_out);

    FRESULT res = f_rename(&self->fatfs, old_path, new_path);
    if (res == FR_EXIST) {
        // if new_path exists then try removing it (but only if it's a file)
        fat_vfs_remove_internal(vfs_in, path_out, 0); // 0 == file attribute
        // try to rename again
        res = f_rename(&self->fatfs, old_path, new_path);
    }
    if (res == FR_OK) {
        return mp_const_none;
    } else {
        mp_raise_OSError_fresult(res);
    }

}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(fat_vfs_rename_obj, fat_vfs_rename);

STATIC mp_obj_t fat_vfs_mkdir(mp_obj_t vfs_in, mp_obj_t path_o) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(vfs_in);
    verify_fs_writable(self);
    const char *path = mp_obj_str_get_str(path_o);
    FRESULT res = f_mkdir(&self->fatfs, path);
    if (res == FR_OK) {
        return mp_const_none;
    } else {
        mp_raise_OSError_fresult(res);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_mkdir_obj, fat_vfs_mkdir);

// Change current directory.
STATIC mp_obj_t fat_vfs_chdir(mp_obj_t vfs_in, mp_obj_t path_in) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(vfs_in);
    const char *path;
    path = mp_obj_str_get_str(path_in);

    FRESULT res = f_chdir(&self->fatfs, path);

    if (res != FR_OK) {
        mp_raise_OSError_fresult(res);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_chdir_obj, fat_vfs_chdir);

// Get the current directory.
STATIC mp_obj_t fat_vfs_getcwd(mp_obj_t vfs_in) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(vfs_in);
    char buf[MICROPY_ALLOC_PATH_MAX + 1];
    FRESULT res = f_getcwd(&self->fatfs, buf, sizeof(buf));
    if (res != FR_OK) {
        mp_raise_OSError_fresult(res);
    }
    return mp_obj_new_str(buf, strlen(buf));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fat_vfs_getcwd_obj, fat_vfs_getcwd);

// Get the status of a file or directory.
STATIC mp_obj_t fat_vfs_stat(mp_obj_t vfs_in, mp_obj_t path_in) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(vfs_in);
    const char *path = mp_obj_str_get_str(path_in);

    FILINFO fno;
    if (path[0] == 0 || (path[0] == '/' && path[1] == 0)) {
        // stat root directory
        fno.fsize = 0;
        fno.fdate = 0x2821; // Jan 1, 2000
        fno.ftime = 0;
        fno.fattrib = AM_DIR;
    } else {
        FRESULT res = f_stat(&self->fatfs, path, &fno);
        if (res != FR_OK) {
            mp_raise_OSError_fresult(res);
        }
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    mp_int_t mode = 0;
    if (fno.fattrib & AM_DIR) {
        mode |= MP_S_IFDIR;
    } else {
        mode |= MP_S_IFREG;
    }
    #if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_NONE
    // On non-longint builds, the number of seconds since 1970 (epoch) is too
    // large to fit in a smallint, so just return 31-DEC-1999 (0).
    mp_obj_t seconds = MP_OBJ_NEW_SMALL_INT(946684800);
    #else
    mp_obj_t seconds = mp_obj_new_int_from_uint(
        timeutils_seconds_since_epoch(
            1980 + ((fno.fdate >> 9) & 0x7f),
            (fno.fdate >> 5) & 0x0f,
            fno.fdate & 0x1f,
            (fno.ftime >> 11) & 0x1f,
            (fno.ftime >> 5) & 0x3f,
            2 * (fno.ftime & 0x1f)
            ));
    #endif
    t->items[0] = MP_OBJ_NEW_SMALL_INT(mode); // st_mode
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0); // st_ino
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // st_dev
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0); // st_nlink
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0); // st_uid
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // st_gid
    t->items[6] = mp_obj_new_int_from_uint(fno.fsize); // st_size
    t->items[7] = seconds; // st_atime
    t->items[8] = seconds; // st_mtime
    t->items[9] = seconds; // st_ctime

    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_stat_obj, fat_vfs_stat);

// Get the status of a VFS.
STATIC mp_obj_t fat_vfs_statvfs(mp_obj_t vfs_in, mp_obj_t path_in) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(vfs_in);
    (void)path_in;

    DWORD nclst;
    FATFS *fatfs = &self->fatfs;
    FRESULT res = f_getfree(fatfs, &nclst);
    if (FR_OK != res) {
        mp_raise_OSError_fresult(res);
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));

    t->items[0] = MP_OBJ_NEW_SMALL_INT(fatfs->csize * SECSIZE(fatfs)); // f_bsize
    t->items[1] = t->items[0]; // f_frsize
    t->items[2] = MP_OBJ_NEW_SMALL_INT((fatfs->n_fatent - 2)); // f_blocks
    t->items[3] = MP_OBJ_NEW_SMALL_INT(nclst); // f_bfree
    t->items[4] = t->items[3]; // f_bavail
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // f_files
    t->items[6] = MP_OBJ_NEW_SMALL_INT(0); // f_ffree
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0); // f_favail
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // f_flags
    t->items[9] = MP_OBJ_NEW_SMALL_INT(FF_MAX_LFN); // f_namemax

    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_statvfs_obj, fat_vfs_statvfs);

STATIC mp_obj_t vfs_fat_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    fs_user_mount_t *self = MP_OBJ_TO_PTR(self_in);

    // Read-only device indicated by writeblocks[0] == MP_OBJ_NULL.
    // User can specify read-only device by:
    //  1. readonly=True keyword argument
    //  2. nonexistent writeblocks method (then writeblocks[0] == MP_OBJ_NULL already)
    if (mp_obj_is_true(readonly)) {
        self->blockdev.writeblocks[0] = MP_OBJ_NULL;
    }

    // check if we need to make the filesystem
    FRESULT res = (self->blockdev.flags & MP_BLOCKDEV_FLAG_NO_FILESYSTEM) ? FR_NO_FILESYSTEM : FR_OK;
    if (res == FR_NO_FILESYSTEM && mp_obj_is_true(mkfs)) {
        uint8_t working_buf[FF_MAX_SS];
        res = f_mkfs(&self->fatfs, FM_FAT | FM_SFD, 0, working_buf, sizeof(working_buf));
    }
    if (res != FR_OK) {
        mp_raise_OSError_fresult(res);
    }
    self->blockdev.flags &= ~MP_BLOCKDEV_FLAG_NO_FILESYSTEM;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_fat_mount_obj, vfs_fat_mount);

STATIC mp_obj_t vfs_fat_umount(mp_obj_t self_in) {
    (void)self_in;
    // keep the FAT filesystem mounted internally so the VFS methods can still be used
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fat_vfs_umount_obj, vfs_fat_umount);

STATIC mp_obj_t vfs_fat_utime(mp_obj_t vfs_in, mp_obj_t path_in, mp_obj_t times_in) {
    mp_obj_fat_vfs_t *self = MP_OBJ_TO_PTR(vfs_in);
    const char *path = mp_obj_str_get_str(path_in);
    if (!mp_obj_is_tuple_compatible(times_in)) {
        mp_raise_type_arg(&mp_type_TypeError, times_in);
    }

    mp_obj_t *otimes;
    mp_obj_get_array_fixed_n(times_in, 2, &otimes);

    // Validate that both elements of the tuple are int and discard the second one
    int time[2];
    time[0] = mp_obj_get_int(otimes[0]);
    time[1] = mp_obj_get_int(otimes[1]);
    timeutils_struct_time_t tm;
    timeutils_seconds_since_epoch_to_struct_time(time[0], &tm);

    FILINFO fno;
    fno.fdate = (WORD)(((tm.tm_year - 1980) * 512U) | tm.tm_mon * 32U | tm.tm_mday);
    fno.ftime = (WORD)(tm.tm_hour * 2048U | tm.tm_min * 32U | tm.tm_sec / 2U);
    FRESULT res = f_utime(&self->fatfs, path, &fno);
    if (res != FR_OK) {
        mp_raise_OSError_fresult(res);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(fat_vfs_utime_obj, vfs_fat_utime);

STATIC mp_obj_t vfs_fat_getreadonly(mp_obj_t self_in) {
    fs_user_mount_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(!filesystem_is_writable_by_python(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fat_vfs_getreadonly_obj, vfs_fat_getreadonly);
STATIC const mp_obj_property_t fat_vfs_readonly_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&fat_vfs_getreadonly_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

#if MICROPY_FATFS_USE_LABEL
STATIC mp_obj_t vfs_fat_getlabel(mp_obj_t self_in) {
    fs_user_mount_t *self = MP_OBJ_TO_PTR(self_in);
    char working_buf[12];
    FRESULT res = f_getlabel(&self->fatfs, working_buf, NULL);
    if (res != FR_OK) {
        mp_raise_OSError_fresult(res);
    }
    return mp_obj_new_str(working_buf, strlen(working_buf));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fat_vfs_getlabel_obj, vfs_fat_getlabel);

STATIC mp_obj_t vfs_fat_setlabel(mp_obj_t self_in, mp_obj_t label_in) {
    fs_user_mount_t *self = MP_OBJ_TO_PTR(self_in);
    verify_fs_writable(self);
    const char *label_str = mp_obj_str_get_str(label_in);
    FRESULT res = f_setlabel(&self->fatfs, label_str);
    if (res != FR_OK) {
        if (res == FR_WRITE_PROTECTED) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Read-only filesystem"));
        }
        mp_raise_OSError_fresult(res);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(fat_vfs_setlabel_obj, vfs_fat_setlabel);
STATIC const mp_obj_property_t fat_vfs_label_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&fat_vfs_getlabel_obj,
              (mp_obj_t)&fat_vfs_setlabel_obj,
              MP_ROM_NONE},
};
#endif

STATIC const mp_rom_map_elem_t fat_vfs_locals_dict_table[] = {
    #if FF_FS_REENTRANT
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&fat_vfs_del_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_mkfs), MP_ROM_PTR(&fat_vfs_mkfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&fat_vfs_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&fat_vfs_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&fat_vfs_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&fat_vfs_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&fat_vfs_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&fat_vfs_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&fat_vfs_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&fat_vfs_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&fat_vfs_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&fat_vfs_statvfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&vfs_fat_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&fat_vfs_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&fat_vfs_utime_obj) },
    { MP_ROM_QSTR(MP_QSTR_readonly), MP_ROM_PTR(&fat_vfs_readonly_obj) },
    #if MICROPY_FATFS_USE_LABEL
    { MP_ROM_QSTR(MP_QSTR_label), MP_ROM_PTR(&fat_vfs_label_obj) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(fat_vfs_locals_dict, fat_vfs_locals_dict_table);

STATIC const mp_vfs_proto_t fat_vfs_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_vfs)
    .import_stat = fat_vfs_import_stat,
};

const mp_obj_type_t mp_fat_vfs_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_VfsFat,
    .make_new = fat_vfs_make_new,
    .locals_dict = (mp_obj_dict_t *)&fat_vfs_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &fat_vfs_proto,
        ),

};

#endif // MICROPY_VFS_FAT
