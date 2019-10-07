/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, NXP
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

#ifndef MICROPY_INCLUDED_ZEPHYR_ZFS_FILE_H
#define MICROPY_INCLUDED_ZEPHYR_ZFS_FILE_H

#include <zephyr.h>
#include <fs/fs.h>

#include "py/obj.h"
#include "py/lexer.h"

typedef struct _zfs_file_t {
    mp_obj_base_t base;
    struct fs_file_t fs_file;
} zfs_file_t;

#if MICROPY_PY_IO_FILEIO
extern const mp_obj_type_t zfs_fileio_type;
#endif
extern const mp_obj_type_t zfs_textio_type;

mp_obj_t zfs_file_open(mp_obj_t path, mp_obj_t mode);
mp_lexer_t *zfs_lexer_new_from_file(const char *filename);

MP_DECLARE_CONST_FUN_OBJ_2(zfs_file_open_obj);

#endif // MICROPY_INCLUDED_ZEPHYR_ZFS_FILE_H
