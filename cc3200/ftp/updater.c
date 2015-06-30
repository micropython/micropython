/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>
#include <stdbool.h>

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "simplelink.h"
#include "flc.h"
#include "updater.h"
#include "shamd5.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "debug.h"

/******************************************************************************
 DEFINE PRIVATE CONSTANTS
 ******************************************************************************/
#define UPDATER_IMG_PATH            "/flash/sys/mcuimg.bin"
#define UPDATER_SRVPACK_PATH        "/flash/sys/servicepack.ucf"
#define UPDATER_SIGN_PATH           "/flash/sys/servicepack.sig"
#define UPDATER_CA_PATH             "/flash/cert/ca.pem"
#define UPDATER_CERT_PATH           "/flash/cert/cert.pem"
#define UPDATER_KEY_PATH            "/flash/cert/private.key"

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct {
    char *path;
    _i32 fhandle;
    _u32 fsize;
    _u32 foffset;
} updater_data_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static updater_data_t updater_data;

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
bool updater_check_path (void *path) {
    if (!strcmp(UPDATER_IMG_PATH, path)) {
        updater_data.path = IMG_UPDATE;
        updater_data.fsize = IMG_SIZE;
    } else if (!strcmp(UPDATER_SRVPACK_PATH, path)) {
        updater_data.path = IMG_SRVPACK;
        updater_data.fsize = SRVPACK_SIZE;
    } else if (!strcmp(UPDATER_SIGN_PATH, path)) {
        updater_data.path = SRVPACK_SIGN;
        updater_data.fsize = SIGN_SIZE;
    } else if (!strcmp(UPDATER_CA_PATH, path)) {
        updater_data.path = CA_FILE;
        updater_data.fsize = CA_KEY_SIZE;
    } else if (!strcmp(UPDATER_CERT_PATH, path)) {
        updater_data.path = CERT_FILE;
        updater_data.fsize = CA_KEY_SIZE;
    } else if (!strcmp(UPDATER_KEY_PATH, path)) {
        updater_data.path = KEY_FILE;
        updater_data.fsize = CA_KEY_SIZE;
    } else {
        return false;
    }
    return true;
}

bool updater_start (void) {
    _u32 AccessModeAndMaxSize = FS_MODE_OPEN_WRITE;
    SlFsFileInfo_t FsFileInfo;
    bool result = false;

    sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
    if (0 != sl_FsGetInfo((_u8 *)updater_data.path, 0, &FsFileInfo)) {
        // file doesn't exist, create it
        AccessModeAndMaxSize = FS_MODE_OPEN_CREATE(updater_data.fsize, 0);
    }
    if (!sl_FsOpen((_u8 *)updater_data.path, AccessModeAndMaxSize, NULL, &updater_data.fhandle)) {
        updater_data.foffset = 0;
        result = true;
    }
    sl_LockObjUnlock (&wlan_LockObj);

    return result;
}

bool updater_write (uint8_t *buf, uint32_t len) {
    bool result = false;

    sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
    if (len == sl_FsWrite(updater_data.fhandle, updater_data.foffset, buf, len)) {
        updater_data.foffset += len;
        result = true;
    }
    sl_LockObjUnlock (&wlan_LockObj);

    return result;
}

void updater_finnish (void) {
    sBootInfo_t sBootInfo;
    _i32 fhandle;

    if (updater_data.fhandle > 0) {
        sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
        // close the file being updated
        sl_FsClose(updater_data.fhandle, NULL, NULL, 0);

        if (!strcmp (IMG_UPDATE, updater_data.path)) {
            // open the boot info file for reading
            if (!sl_FsOpen((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_READ, NULL, &fhandle)) {
                ASSERT (sizeof(sBootInfo_t) == sl_FsRead(fhandle, 0, (unsigned char *)&sBootInfo, sizeof(sBootInfo_t)));
                sl_FsClose(fhandle, 0, 0, 0);
                // open the file for writing
                ASSERT (sl_FsOpen((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_WRITE, NULL, &fhandle) == 0);
            }
            else {
                _u32 BootInfoCreateFlag  = _FS_FILE_OPEN_FLAG_COMMIT | _FS_FILE_PUBLIC_WRITE | _FS_FILE_PUBLIC_READ;
                ASSERT (sl_FsOpen ((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_CREATE((2 * sizeof(sBootInfo_t)),
                                   BootInfoCreateFlag), NULL, &fhandle) == 0);
            }

            // write the new boot info
            sBootInfo.ActiveImg = IMG_ACT_UPDATE;
            sBootInfo.Status = IMG_STATUS_CHECK;
            ASSERT (sizeof(sBootInfo_t) == sl_FsWrite(fhandle, 0, (unsigned char *)&sBootInfo, sizeof(sBootInfo_t)));
            sl_FsClose(fhandle, 0, 0, 0);
        }
        sl_LockObjUnlock (&wlan_LockObj);
    }
    updater_data.fhandle = -1;
}
