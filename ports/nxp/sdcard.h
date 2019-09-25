/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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

#ifndef _SDCARD_H_
#define _SDCARD_H_

#include "fsl_sd.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/binary.h"

#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"

status_t ReadBlocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks);
status_t WriteBlocks(const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);
status_t sdcard_WaitCardInsert(void);
bool disk_initialize(void);

extern const mp_obj_type_t pyb_sdcard_type;
extern const mp_obj_base_t pyb_sdcard_obj;
extern sd_card_t g_sd;

struct _fs_user_mount_t;
void sdcard_init_vfs(struct _fs_user_mount_t *vfs, int part);

#endif