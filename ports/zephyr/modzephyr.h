/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 NXP
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

#ifndef MICROPY_INCLUDED_ZEPHYR_MODZEPHYR_H
#define MICROPY_INCLUDED_ZEPHYR_MODZEPHYR_H

#include "py/obj.h"

#ifdef CONFIG_DISK_ACCESS
extern const mp_obj_type_t zephyr_disk_access_type;
#endif

#ifdef CONFIG_FLASH_MAP
extern const mp_obj_type_t zephyr_flash_area_type;
#endif

#ifdef CONFIG_FILE_SYSTEM
extern const mp_obj_type_t zephyr_fs_type;
size_t mp_zephyr_fs_get_mounts_size(void);
struct fs_mount_t *const *mp_zephyr_fs_get_mounts(void);
#endif

#endif // MICROPY_INCLUDED_ZEPHYR_MODZEPHYR_H
