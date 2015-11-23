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

#include "py/mpstate.h"
#include "lib/fatfs/ff.h"
#include "lib/fatfs/ffconf.h"
#include "lib/fatfs/diskio.h"
#include "moduos.h"

#if _FS_RPATH
extern BYTE ff_CurrVol;
#endif

STATIC bool check_path(const TCHAR **path, const char *mount_point_str, mp_uint_t mount_point_len) {
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

    if (check_path(path, "/flash", 6)) {
        return PD_FLASH;
    }
    else {
        for (mp_uint_t i = 0; i < MP_STATE_PORT(mount_obj_list).len; i++) {
            os_fs_mount_t *mount_obj = ((os_fs_mount_t *)(MP_STATE_PORT(mount_obj_list).items[i]));
            if (check_path(path, mount_obj->path, mount_obj->pathlen)) {
                return mount_obj->vol;
            }
        }
    }

    return -1;
}

void ff_get_volname(BYTE vol, TCHAR **dest) {
    if (vol == PD_FLASH) {
        memcpy(*dest, "/flash", 6);
        *dest += 6;
    } else {
        os_fs_mount_t *mount_obj;
        if ((mount_obj = osmount_find_by_volume(vol))) {
            memcpy(*dest, mount_obj->path, mount_obj->pathlen);
            *dest += mount_obj->pathlen;
        }
    }
}
