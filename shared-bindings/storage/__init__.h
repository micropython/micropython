/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_STORAGE___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_STORAGE___INIT___H

#include <stdint.h>
#include <stdbool.h>

#include "shared-module/storage/__init__.h"

void common_hal_storage_mount(mp_obj_t vfs_obj, const char *path, bool readonly);
void common_hal_storage_umount_path(const char *path);
void common_hal_storage_umount_object(mp_obj_t vfs_obj);
void common_hal_storage_remount(const char *path, bool readonly, bool disable_concurrent_write_protection);
mp_obj_t common_hal_storage_getmount(const char *path);
void common_hal_storage_erase_filesystem(void);

bool common_hal_storage_disable_usb_drive(void);
bool common_hal_storage_enable_usb_drive(void);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_STORAGE___INIT___H
