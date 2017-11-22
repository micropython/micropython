/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"
#include "py/lexer.h"

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

        // make 3-tuple with info about this entry
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
        if (self->is_str) {
            t->items[0] = mp_obj_new_str(fn, strlen(fn));
        } else {
            t->items[0] = mp_obj_new_bytes((const byte*)fn, strlen(fn));
        }
        if (fno.fattrib & AM_DIR) {
            // dir
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR);
        } else {
            // file
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG);
        }
        t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // no inode number

        return MP_OBJ_FROM_PTR(t);
    }

    // ignore error because we may be closing a second time
    f_closedir(&self->dir);

    return MP_OBJ_STOP_ITERATION;
}

mp_obj_t fat_vfs_ilistdir2(fs_user_mount_t *vfs, const char *path, bool is_str_type) {
    mp_vfs_fat_ilistdir_it_t *iter = m_new_obj(mp_vfs_fat_ilistdir_it_t);
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = mp_vfs_fat_ilistdir_it_iternext;
    iter->is_str = is_str_type;
    FRESULT res = f_opendir(&vfs->fatfs, &iter->dir, path);
    if (res != FR_OK) {
        mp_raise_OSError(fresult_to_errno_table[res]);
    }
    return MP_OBJ_FROM_PTR(iter);
}

mp_import_stat_t fat_vfs_import_stat(fs_user_mount_t *vfs, const char *path) {
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

#endif // MICROPY_VFS_FAT
