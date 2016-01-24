/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Dave Hylands
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

#ifndef __MICROPY_INCLUDED_PY_MPFILE_H__
#define __MICROPY_INCLUDED_PY_MPFILE_H__

#include "py/obj.h"
#include <sys/types.h>  // for off_t

// A C API for performing I/O on files or file-like objects.

typedef struct {
    mp_obj_base_t   base;
    mp_obj_t        file_obj;
    mp_obj_t        readinto_fn;
    mp_obj_t        seek_fn;
    mp_obj_t        tell_fn;
} mp_file_t;

#define MP_SEEK_SET 0
#define MP_SEEK_CUR 1
#define MP_SEEK_END 2

mp_file_t *mp_file_from_file_obj(mp_obj_t file_obj);
mp_file_t *mp_open(const char *filename, const char *mode);
mp_int_t mp_readinto(mp_file_t *file, void *buf, size_t num_bytes);
off_t mp_seek(mp_file_t *file, off_t offset, int whence);
off_t mp_tell(mp_file_t *file);
void mp_close(mp_file_t *file);


#endif // __MICROPY_INCLUDED_PY_MPFILE_H__
