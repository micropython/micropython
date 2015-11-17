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

#ifndef MODUOS_H_
#define MODUOS_H_

#include "ff.h"

/******************************************************************************
 DEFINE PUBLIC TYPES
 ******************************************************************************/
typedef struct _os_fs_mount_t {
    mp_obj_t device;
    const char *path;
    mp_uint_t pathlen;
    mp_obj_t readblocks[4];
    mp_obj_t writeblocks[4];
    mp_obj_t sync[2];
    mp_obj_t count[2];
    FATFS fatfs;
    uint8_t vol;
} os_fs_mount_t;

typedef struct _os_term_dup_obj_t {
    mp_obj_t stream_o;
    mp_obj_t read[3];
    mp_obj_t write[3];
} os_term_dup_obj_t;

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/
void moduos_init0 (void);
os_fs_mount_t *osmount_find_by_path (const char *path);
os_fs_mount_t *osmount_find_by_volume (uint8_t vol);
void osmount_unmount_all (void);

#endif // MODUOS_H_
