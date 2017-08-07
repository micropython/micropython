/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include "py/obj.h"
#include "simplelink.h"
#include "flc.h"
#include "updater.h"
#include "shamd5.h"
#include "modnetwork.h"
#include "modwlan.h"
#include "debug.h"
#include "osi.h"

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
static updater_data_t updater_data = { .path = NULL, .fhandle = -1, .fsize = 0, .foffset = 0 };
static OsiLockObj_t updater_LockObj;
static sBootInfo_t sBootInfo;

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
__attribute__ ((section (".boot")))
void updater_pre_init (void) {
    // create the updater lock
    ASSERT(OSI_OK == sl_LockObjCreate(&updater_LockObj, "UpdaterLock"));
}

bool updater_check_path (void *path) {
    sl_LockObjLock (&updater_LockObj, SL_OS_WAIT_FOREVER);
    if (!strcmp(UPDATER_IMG_PATH, path)) {
        updater_data.fsize = IMG_SIZE;
        updater_data.path = IMG_UPDATE1;
// the launchxl doesn't have enough flash space for 2 user update images
#ifdef WIPY
        // check which one should be the next active image
         _i32 fhandle;
        if (!sl_FsOpen((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_READ, NULL, &fhandle)) {
            ASSERT (sizeof(sBootInfo_t) == sl_FsRead(fhandle, 0, (unsigned char *)&sBootInfo, sizeof(sBootInfo_t)));
            sl_FsClose(fhandle, 0, 0, 0);
            // if we still have an image pending for verification, keep overwriting it
            if ((sBootInfo.Status == IMG_STATUS_CHECK && sBootInfo.ActiveImg == IMG_ACT_UPDATE2) ||
                 (sBootInfo.ActiveImg == IMG_ACT_UPDATE1 && sBootInfo.Status != IMG_STATUS_CHECK)) {
                updater_data.path = IMG_UPDATE2;
            }
        }
#endif
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
        sl_LockObjUnlock (&updater_LockObj);
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
    _i32 fhandle;

    if (updater_data.fhandle > 0) {
        sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
        // close the file being updated
        sl_FsClose(updater_data.fhandle, NULL, NULL, 0);
#ifdef WIPY
        // if we still have an image pending for verification, leave the boot info as it is
        if (!strncmp(IMG_PREFIX, updater_data.path, strlen(IMG_PREFIX)) && sBootInfo.Status != IMG_STATUS_CHECK) {
#else
        if (!strncmp(IMG_PREFIX, updater_data.path, strlen(IMG_PREFIX))) {
#endif
#ifdef DEBUG
            if (!sl_FsOpen((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_READ, NULL, &fhandle)) {

                ASSERT (sizeof(sBootInfo_t) == sl_FsRead(fhandle, 0, (unsigned char *)&sBootInfo, sizeof(sBootInfo_t)));
                sl_FsClose(fhandle, 0, 0, 0);
#endif
                // open the boot info file for writing
                ASSERT (sl_FsOpen((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_WRITE, NULL, &fhandle) == 0);
#ifdef DEBUG
            }
            else {
                // the boot info file doesn't exist yet
                _u32 BootInfoCreateFlag  = _FS_FILE_OPEN_FLAG_COMMIT | _FS_FILE_PUBLIC_WRITE | _FS_FILE_PUBLIC_READ;
                ASSERT (sl_FsOpen ((unsigned char *)IMG_BOOT_INFO, FS_MODE_OPEN_CREATE((2 * sizeof(sBootInfo_t)),
                                   BootInfoCreateFlag), NULL, &fhandle) == 0);
            }
#endif

            // save the new boot info
#ifdef WIPY
            sBootInfo.PrevImg = sBootInfo.ActiveImg;
            if (sBootInfo.ActiveImg == IMG_ACT_UPDATE1) {
                sBootInfo.ActiveImg = IMG_ACT_UPDATE2;
            } else {
                sBootInfo.ActiveImg = IMG_ACT_UPDATE1;
            }
// the launchxl doesn't have enough flash space for 2 user updates
#else
            sBootInfo.PrevImg = IMG_ACT_FACTORY;
            sBootInfo.ActiveImg = IMG_ACT_UPDATE1;
#endif
            sBootInfo.Status = IMG_STATUS_CHECK;
            ASSERT (sizeof(sBootInfo_t) == sl_FsWrite(fhandle, 0, (unsigned char *)&sBootInfo, sizeof(sBootInfo_t)));
            sl_FsClose(fhandle, 0, 0, 0);
        }
        sl_LockObjUnlock (&wlan_LockObj);
        updater_data.fhandle = -1;
    }
    sl_LockObjUnlock (&updater_LockObj);
}

