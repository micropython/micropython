/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"
#include "py/mpstate.h"
#include "supervisor/filesystem.h"

void board_init(void) {
    mp_import_stat_t stat_b = mp_import_stat("boot.py");
    if (stat_b != MP_IMPORT_STAT_FILE) {
        FATFS *fatfs = filesystem_circuitpy();
        FIL fs;
        UINT char_written = 0;
        const byte buffer[] = "#Serial port upload mode\nimport storage\nstorage.remount(\"/\", False)\nstorage.disable_usb_drive()\n";
        // Create or modify existing boot.py file
        f_open(fatfs, &fs, "/boot.py", FA_WRITE | FA_CREATE_ALWAYS);
        f_write(&fs, buffer, sizeof(buffer) - 1, &char_written);
        f_close(&fs);
        // Delete code.Py, use main.py
        mp_import_stat_t stat_c = mp_import_stat("code.py");
        if (stat_c == MP_IMPORT_STAT_FILE) {
            f_unlink(fatfs, "/code.py");
        }
    }

}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
