/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <string.h>
#include <std.h>

#include "py/mpconfig.h"
#include "py/misc.h"
#include "ff.h"
#include "ffconf.h"
#include "diskio.h"

#if _FS_RPATH
extern BYTE ff_CurrVol;
#endif

STATIC bool check_path(const TCHAR **path, const char *mount_point_str, mp_uint_t mount_point_len) {
    stoupper ((char *)(*path));
    if (strncmp(*path, mount_point_str, mount_point_len) == 0) {
        if ((*path)[mount_point_len] == '/') {
            *path += mount_point_len;
            return true;
        } else if ((*path)[mount_point_len] == '\0') {
            *path = "/";
            return true;
        }
    }
    return false;
}

// "path" is the path to lookup; will advance this pointer beyond the volume name.
// Returns logical drive number (-1 means invalid path).
int ff_get_ldnumber (const TCHAR **path) {
    if (!(*path)) {
        return -1;
    }

    if (**path != '/') {
    #if _FS_RPATH
        return ff_CurrVol;
    #else
        return -1;
    #endif
    }

    if (check_path(path, "/SFLASH", 7)) {
        return 0;
    }
#if MICROPY_HW_HAS_SDCARD
    else if (check_path(path, "/SD", 3)) {
        return 1;
    }
#endif
    else {
        return -1;
    }
}

void ff_get_volname(BYTE vol, TCHAR **dest) {
#if MICROPY_HW_HAS_SDCARD
    if (vol == 0)
#endif
    {
        memcpy(*dest, "/SFLASH", 7);
        *dest += 7;
    }
#if MICROPY_HW_HAS_SDCARD
    else
    {
        memcpy(*dest, "/SD", 3);
        *dest += 3;
    }
#endif
}
