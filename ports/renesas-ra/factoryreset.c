/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 * Copyright (c) 2022 Renesas Electronics Corporation
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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/vfs_fat.h"
#include "systick.h"
#include "led.h"
#include "storage.h"
#include "factoryreset.h"

#if MICROPY_HW_ENABLE_STORAGE

#if MICROPY_VFS_FAT

static const char fresh_boot_py[] =
    "# boot.py -- run on boot-up\r\n"
    "# can run arbitrary Python, but best to keep it minimal\r\n"
;

static const char fresh_main_py[] =
    "# main.py -- put your code here!\r\n"
;

typedef struct _factory_file_t {
    const char *name;
    size_t len;
    const char *data;
} factory_file_t;

static const factory_file_t factory_files[] = {
    {"boot.py", sizeof(fresh_boot_py) - 1, fresh_boot_py},
    {"main.py", sizeof(fresh_main_py) - 1, fresh_main_py},
};

MP_WEAK void factory_reset_make_files(FATFS *fatfs) {
    for (int i = 0; i < MP_ARRAY_SIZE(factory_files); ++i) {
        const factory_file_t *f = &factory_files[i];
        FIL fp;
        FRESULT res = f_open(fatfs, &fp, f->name, FA_WRITE | FA_CREATE_ALWAYS);
        if (res == FR_OK) {
            UINT n;
            f_write(&fp, f->data, f->len, &n);
            f_close(&fp);
        }
    }
}

MP_WEAK int factory_reset_create_filesystem(void) {
    // LED on to indicate creation of local filesystem
    led_state(RA_LED1, 1);
    uint32_t start_tick = HAL_GetTick();

    fs_user_mount_t vfs;
    pyb_flash_init_vfs(&vfs);
    uint8_t working_buf[FF_MAX_SS];
    FRESULT res = f_mkfs(&vfs.fatfs, FM_FAT, 0, working_buf, sizeof(working_buf));
    if (res != FR_OK) {
        mp_printf(&mp_plat_print, "MPY: can't create flash filesystem\n");
        return -MP_ENODEV;
    }

    // Set label
    f_setlabel(&vfs.fatfs, MICROPY_HW_FLASH_FS_LABEL);

    // Populate the filesystem with factory files
    factory_reset_make_files(&vfs.fatfs);

    // Keep LED on for at least 200ms
    systick_wait_at_least(start_tick, 200);
    led_state(RA_LED1, 0);

    mp_printf(&mp_plat_print, "MPY: created flash filesystem\n");

    return 0; // success
}

#else

// If FAT is not enabled then it's up to the board to create a fresh filesystem.
MP_WEAK int factory_reset_create_filesystem(void) {
    return 0; // success
}

#endif // MICROPY_VFS_FAT

#endif // MICROPY_HW_ENABLE_STORAGE
