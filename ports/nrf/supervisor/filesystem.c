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

#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"

#include "py/mpstate.h"

#include "internal_flash.h"

static mp_vfs_mount_t _mp_vfs;
static fs_user_mount_t _internal_vfs;


void filesystem_init(bool create_allowed) {
    // init the vfs object
    fs_user_mount_t *int_vfs = &_internal_vfs;
    int_vfs->flags = 0;
    flash_init_vfs(int_vfs);

    // try to mount the flash
    FRESULT res = f_mount(&int_vfs->fatfs);

    if (res == FR_NO_FILESYSTEM && create_allowed) {
        // no filesystem so create a fresh one
        uint8_t working_buf[_MAX_SS];
        res = f_mkfs(&int_vfs->fatfs, FM_FAT | FM_SFD, 4096, working_buf, sizeof(working_buf));
        // Flush the new file system to make sure its repaired immediately.
        flash_flush();
        if (res != FR_OK) {
            return;
        }

        // set label
        f_setlabel(&int_vfs->fatfs, "CIRCUITPY");

        // create lib folder
        f_mkdir(&int_vfs->fatfs, "/lib");

        flash_flush();
    } else if (res != FR_OK) {
        return;
    }

    mp_vfs_mount_t *mp_vfs = &_mp_vfs;
    mp_vfs->str = "/";
    mp_vfs->len = 1;
    mp_vfs->obj = MP_OBJ_FROM_PTR(int_vfs);
    mp_vfs->next = NULL;
    MP_STATE_VM(vfs_mount_table) = mp_vfs;

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    MP_STATE_PORT(vfs_cur) = mp_vfs;
}

void filesystem_flush(void) {
    flash_flush();
}

void filesystem_writable_by_python(bool writable) {
    fs_user_mount_t *vfs = &_internal_vfs;

    if (writable) {
        vfs->flags |= FSUSER_USB_WRITABLE;
    } else {
        vfs->flags &= ~FSUSER_USB_WRITABLE;
    }
}

bool filesystem_present(void) {
    return true;
}
