#include <stdint.h>
#include <stdbool.h>
#include "std.h"

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "simplelink.h"
#include "flc.h"
#include "updater.h"
#include "shamd5.h"
#include "modwlan.h"
#include "debug.h"

/******************************************************************************
 DEFINE PRIVATE CONSTANTS
 ******************************************************************************/
#define UPDATER_IMG_PATH            "/SFLASH/SYS/MCUIMG.BIN"
#define UPDATER_SRVPACK_PATH        "/SFLASH/SYS/SRVPCK.UCF"
#define UPDATER_SIGN_PATH           "/SFLASH/SYS/SRVPCK.SIG"

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
    // conert the path supplied to upper case
    stoupper (path);
    if (!strcmp(UPDATER_IMG_PATH, path)) {
        updater_data.path = IMG_UPDATE;
        updater_data.fsize = IMG_SIZE;
        return true;
    }
    else if (!strcmp(UPDATER_SRVPACK_PATH, path)) {
        updater_data.path = IMG_SRVPACK;
        updater_data.fsize = SRVPACK_SIZE;
        return true;
    }
    else if (!strcmp(UPDATER_SIGN_PATH, path)) {
        updater_data.path = SRVPACK_SIGN;
        updater_data.fsize = SIGN_SIZE;
        return true;
    }
    return false;
}

bool updater_start (void) {
    _u32 AccessModeAndMaxSize = FS_MODE_OPEN_WRITE;
    SlFsFileInfo_t FsFileInfo;
    sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
    if (0 != sl_FsGetInfo((_u8 *)updater_data.path, 0, &FsFileInfo)) {
        // file doesn't exist, create it
        AccessModeAndMaxSize = FS_MODE_OPEN_CREATE(updater_data.fsize, 0);
    }
    if (!sl_FsOpen((_u8 *)updater_data.path, AccessModeAndMaxSize, NULL, &updater_data.fhandle)) {
        updater_data.foffset = 0;
        return true;
    }
    sl_LockObjUnlock (&wlan_LockObj);
    return false;
}

bool updater_write (uint8_t *buf, uint32_t len) {
    if (len == sl_FsWrite(updater_data.fhandle, updater_data.foffset, buf, len)) {
        updater_data.foffset += len;
        return true;
    }
    return false;
}

void updater_finnish (void) {
    sBootInfo_t sBootInfo;
    _i32 fhandle;

    if (updater_data.fhandle > 0) {
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
    }
    updater_data.fhandle = -1;
    sl_LockObjUnlock (&wlan_LockObj);
}
