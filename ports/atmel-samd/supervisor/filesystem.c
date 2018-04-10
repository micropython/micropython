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

#include "flash_api.h"

#ifdef EXPRESS_BOARD
// #include "common-hal/touchio/TouchIn.h"
#define INTERNAL_CIRCUITPY_CONFIG_START_ADDR (0x00040000 - 0x100 - CIRCUITPY_INTERNAL_NVM_SIZE)
#else
#define INTERNAL_CIRCUITPY_CONFIG_START_ADDR (0x00040000 - 0x010000 - 0x100 - CIRCUITPY_INTERNAL_NVM_SIZE)
#endif

fs_user_mount_t fs_user_mount_flash;
mp_vfs_mount_t mp_vfs_mount_flash;

static void make_empty_file(FATFS *fatfs, const char *path) {
    FIL fp;
    f_open(fatfs, &fp, path, FA_WRITE | FA_CREATE_ALWAYS);
    f_close(&fp);
}

// we don't make this function static because it needs a lot of stack and we
// want it to be executed without using stack within main() function
void filesystem_init(bool create_allowed, bool force_create) {
    // init the vfs object
    fs_user_mount_t *vfs_fat = &fs_user_mount_flash;
    vfs_fat->flags = 0;
    flash_init_vfs(vfs_fat);

    // try to mount the flash
    FRESULT res = f_mount(&vfs_fat->fatfs);

    if ((res == FR_NO_FILESYSTEM && create_allowed) || force_create) {
        // No filesystem so create a fresh one, or reformat has been requested.
        uint8_t working_buf[_MAX_SS];
        res = f_mkfs(&vfs_fat->fatfs, FM_FAT, 0, working_buf, sizeof(working_buf));
        // Flush the new file system to make sure it's repaired immediately.
        flash_flush();
        if (res != FR_OK) {
            return;
        }

        // set label
        f_setlabel(&vfs_fat->fatfs, "CIRCUITPY");

        // inhibit file indexing on MacOS
        f_mkdir(&vfs_fat->fatfs, "/.fseventsd");
        make_empty_file(&vfs_fat->fatfs, "/.metadata_never_index");
        make_empty_file(&vfs_fat->fatfs, "/.Trashes");
        make_empty_file(&vfs_fat->fatfs, "/.fseventsd/no_log");

        // and ensure everything is flushed
        flash_flush();
    } else if (res != FR_OK) {
        return;
    }
    mp_vfs_mount_t *vfs = &mp_vfs_mount_flash;
    vfs->str = "/";
    vfs->len = 1;
    vfs->obj = MP_OBJ_FROM_PTR(vfs_fat);
    vfs->next = NULL;
    MP_STATE_VM(vfs_mount_table) = vfs;

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    MP_STATE_PORT(vfs_cur) = vfs;
}

void filesystem_flush(void) {
    flash_flush();
}

void filesystem_writable_by_python(bool writable) {
    flash_set_usb_writable(!writable);
}

bool filesystem_present(void) {
    return true;
}
