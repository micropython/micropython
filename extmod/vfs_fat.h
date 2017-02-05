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

#include "py/lexer.h"
#include "py/obj.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs.h"

// these are the values for fs_user_mount_t.flags
#define FSUSER_NATIVE       (0x0001) // readblocks[2]/writeblocks[2] contain native func
#define FSUSER_FREE_OBJ     (0x0002) // fs_user_mount_t obj should be freed on umount
#define FSUSER_HAVE_IOCTL   (0x0004) // new protocol with ioctl

typedef struct _fs_user_mount_t {
    mp_obj_base_t base;
    const char *str;
    uint16_t len; // length of str
    uint16_t flags;
    mp_obj_t readblocks[4];
    mp_obj_t writeblocks[4];
    // new protocol uses just ioctl, old uses sync (optional) and count
    union {
        mp_obj_t ioctl[4];
        struct {
            mp_obj_t sync[2];
            mp_obj_t count[2];
        } old;
    } u;
    FATFS fatfs;
} fs_user_mount_t;

extern const byte fresult_to_errno_table[20];
extern const mp_obj_type_t mp_fat_vfs_type;

mp_import_stat_t fat_vfs_import_stat(struct _fs_user_mount_t *vfs, const char *path);
mp_obj_t fatfs_builtin_open_self(mp_obj_t self_in, mp_obj_t path, mp_obj_t mode);
MP_DECLARE_CONST_FUN_OBJ_KW(mp_builtin_open_obj);

mp_obj_t fat_vfs_listdir2(struct _fs_user_mount_t *vfs, const char *path, bool is_str_type);
