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

#include "supervisor/filesystem.h"


void filesystem_background(void) {
    return;
}

void filesystem_tick(void) {
    return;
}

bool filesystem_init(bool create_allowed, bool force_create) {
    (void)create_allowed;
    (void)force_create;
    return true;
}

void filesystem_flush(void) {
}

void filesystem_set_internal_writable_by_usb(bool writable) {
    (void)writable;
    return;
}

void filesystem_set_writable_by_usb(fs_user_mount_t *vfs, bool usb_writable) {
    (void)vfs;
    (void)usb_writable;
    return;
}

bool filesystem_is_writable_by_usb(fs_user_mount_t *vfs) {
    return true;
}

void filesystem_set_internal_concurrent_write_protection(bool concurrent_write_protection) {
    (void)concurrent_write_protection;
    return;
}

void filesystem_set_concurrent_write_protection(fs_user_mount_t *vfs, bool concurrent_write_protection) {
    (void)vfs;
    (void)concurrent_write_protection;
    return;
}

bool filesystem_present(void) {
    return false;
}

bool filesystem_lock(fs_user_mount_t *fs_mount) {
    if (fs_mount->lock_count == 0 && !blockdev_lock(fs_mount)) {
        return false;
    }
    fs_mount->lock_count += 1;
    return true;
}

void filesystem_unlock(fs_user_mount_t *fs_mount) {
    assert(fs_mount->lock_count > 0);
    fs_mount->lock_count -= 1;
    if (fs_mount->lock_count == 0) {
        blockdev_unlock(fs_mount);
    }
}

bool blockdev_lock(fs_user_mount_t *fs_mount) {
    if ((fs_mount->blockdev.flags & MP_BLOCKDEV_FLAG_LOCKED) != 0) {
        return false;
    }
    fs_mount->blockdev.flags |= MP_BLOCKDEV_FLAG_LOCKED;
    return true;
}

void blockdev_unlock(fs_user_mount_t *fs_mount) {
    assert(fs_mount->blockdev.flags & MP_BLOCKDEV_FLAG_LOCKED);
    fs_mount->blockdev.flags &= ~MP_BLOCKDEV_FLAG_LOCKED;
}
