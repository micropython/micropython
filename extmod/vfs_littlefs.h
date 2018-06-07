/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
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
#ifndef MICROPY_INCLUDED_EXTMOD_VFS_LITTLEFS_H
#define MICROPY_INCLUDED_EXTMOD_VFS_LITTLEFS_H

#include "py/obj.h"
#include "lib/littlefs/lfs.h"

typedef struct _mp_obj_vfs_littlefs_t {
    mp_obj_base_t base;
    vstr_t cur_dir;
    struct lfs_config config;
    lfs_t lfs;
} mp_obj_vfs_littlefs_t;

extern const mp_obj_type_t mp_type_vfs_littlefs;
extern const mp_obj_type_t mp_type_vfs_littlefs_fileio;
extern const mp_obj_type_t mp_type_vfs_littlefs_textio;

const char *mp_obj_vfs_littlefs_make_path(mp_obj_vfs_littlefs_t *self, mp_obj_t path_in);
mp_obj_t mp_vfs_littlefs_file_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in);

#endif // MICROPY_INCLUDED_EXTMOD_VFS_LITTLEFS_H
