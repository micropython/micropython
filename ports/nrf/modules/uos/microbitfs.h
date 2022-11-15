/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Mark Shannon
 * Copyright (c) 2017 Ayke van Laethem
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
#ifndef __MICROPY_INCLUDED_FILESYSTEM_H__
#define __MICROPY_INCLUDED_FILESYSTEM_H__

#include "py/builtin.h"
#include "py/obj.h"
#include "py/lexer.h"

#ifndef MBFS_LOG_CHUNK_SIZE
// This property can be tuned to make the filesystem bigger (while keeping
// the max number of blocks). Note that it cannot (currently) be increased
// beyond 8 or uint8_t integers will overflow.
// 2^7 == 128 bytes
// (128-2) bytes/block * 252 blocks = 31752 usable bytes
#define MBFS_LOG_CHUNK_SIZE 7
#endif

mp_obj_t uos_mbfs_open(size_t n_args, const mp_obj_t *args);
void microbit_filesystem_init(void);
mp_lexer_t *uos_mbfs_new_reader(const char *filename);
mp_import_stat_t uos_mbfs_import_stat(const char *path);

MP_DECLARE_CONST_FUN_OBJ_0(uos_mbfs_listdir_obj);
MP_DECLARE_CONST_FUN_OBJ_0(uos_mbfs_ilistdir_obj);
MP_DECLARE_CONST_FUN_OBJ_1(uos_mbfs_remove_obj);
MP_DECLARE_CONST_FUN_OBJ_1(uos_mbfs_stat_obj);

#endif // __MICROPY_INCLUDED_FILESYSTEM_H__
