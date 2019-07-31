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

#ifndef MICROPY_INCLUDED_SUPERVISOR_FILESYSTEM_H
#define MICROPY_INCLUDED_SUPERVISOR_FILESYSTEM_H

#include <stdbool.h>

#include "extmod/vfs_fat.h"

extern volatile bool filesystem_flush_requested;

void filesystem_background(void);
void filesystem_tick(void);
void filesystem_init(bool create_allowed, bool force_create);
void filesystem_flush(void);
bool filesystem_present(void);
void filesystem_set_internal_writable_by_usb(bool usb_writable);
void filesystem_set_internal_concurrent_write_protection(bool concurrent_write_protection);
void filesystem_set_writable_by_usb(fs_user_mount_t *vfs, bool usb_writable);
void filesystem_set_concurrent_write_protection(fs_user_mount_t *vfs, bool concurrent_write_protection);
bool filesystem_is_writable_by_python(fs_user_mount_t *vfs);
bool filesystem_is_writable_by_usb(fs_user_mount_t *vfs);

#endif  // MICROPY_INCLUDED_SUPERVISOR_FILESYSTEM_H
