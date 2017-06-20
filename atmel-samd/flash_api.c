/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "flash_api.h"

#include "py/mpstate.h"

#define VFS_INDEX 0

void flash_set_usb_writeable(bool usb_writeable) {
    mp_vfs_mount_t* current_mount = MP_STATE_VM(vfs_mount_table);
    for (uint8_t i = 0; current_mount != NULL; i++) {
        if (i == VFS_INDEX) {
            break;
        }
        current_mount = current_mount->next;
    }
    if (current_mount == NULL) {
        return;
    }
    fs_user_mount_t *vfs = (fs_user_mount_t *) current_mount->obj;

    if (usb_writeable) {
        vfs->flags |= FSUSER_USB_WRITEABLE;
    } else {
        vfs->flags &= ~FSUSER_USB_WRITEABLE;
    }
}
