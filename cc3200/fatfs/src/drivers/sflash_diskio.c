#include <stdint.h>
#include <stdbool.h>
#include "std.h"

#include "py/mpconfig.h"
#include "py/obj.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "simplelink.h"
#include "sflash_diskio.h"
#include "debug.h"
#include "modnetwork.h"
#include "modwlan.h"

#define SFLASH_TIMEOUT_MAX_MS               5500
#define SFLASH_WAIT_TIME_MS                 5

static _u8 sflash_block_name[] =  "__NNN__.fsb";
static _u8 *sflash_block_cache;
static bool sflash_init_done = false;
static bool sflash_cache_is_dirty;
static uint32_t sflash_ublock;
static uint32_t sflash_prblock;


static void print_block_name (_u32 ublock) {
    char _sblock[4];
    snprintf (_sblock, sizeof(_sblock), "%03u", ublock);
    memcpy (&sflash_block_name[2], _sblock, 3);
}

static bool sflash_access (_u32 mode, _i32 (* sl_FsFunction)(_i32 FileHdl, _u32 Offset, _u8* pData, _u32 Len)) {
    _i32 fileHandle;
    bool retval = false;

    // wlan must be enabled in order to access the serial flash
    sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);

    if (0 == sl_FsOpen(sflash_block_name, mode, NULL, &fileHandle)) {
        if (SFLASH_BLOCK_SIZE == sl_FsFunction (fileHandle, 0, sflash_block_cache, SFLASH_BLOCK_SIZE)) {
            retval = true;
        }
        sl_FsClose (fileHandle, NULL, NULL, 0);
    }
    sl_LockObjUnlock (&wlan_LockObj);
    return retval;
}

DRESULT sflash_disk_init (void) {
    _i32 fileHandle;
    SlFsFileInfo_t FsFileInfo;

    if (!sflash_init_done) {
        // Allocate space for the block cache
        ASSERT ((sflash_block_cache = mem_Malloc(SFLASH_BLOCK_SIZE)) != NULL);
        sflash_init_done = true;
        sflash_prblock = UINT32_MAX;
        sflash_cache_is_dirty = false;

        // In order too speed up booting, check the last block, if exists, then
        // it means that the file system has been already created
        print_block_name (SFLASH_BLOCK_COUNT - 1);
        sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
        if (!sl_FsGetInfo(sflash_block_name, 0, &FsFileInfo)) {
            sl_LockObjUnlock (&wlan_LockObj);
            return RES_OK;
        }
        sl_LockObjUnlock (&wlan_LockObj);

        // Proceed to format the memory
        for (int i = 0; i < SFLASH_BLOCK_COUNT; i++) {
            print_block_name (i);
            sl_LockObjLock (&wlan_LockObj, SL_OS_WAIT_FOREVER);
            // Create the block file if it doesn't exist
            if (sl_FsGetInfo(sflash_block_name, 0, &FsFileInfo) != 0) {
                if (!sl_FsOpen(sflash_block_name, FS_MODE_OPEN_CREATE(SFLASH_BLOCK_SIZE, 0), NULL, &fileHandle)) {
                    sl_FsClose(fileHandle, NULL, NULL, 0);
                    sl_LockObjUnlock (&wlan_LockObj);
                    memset(sflash_block_cache, 0xFF, SFLASH_BLOCK_SIZE);
                    if (!sflash_access(FS_MODE_OPEN_WRITE, sl_FsWrite)) {
                        return RES_ERROR;
                    }
                }
                else {
                    // Unexpected failure while creating the file
                    sl_LockObjUnlock (&wlan_LockObj);
                    return RES_ERROR;
                }
            }
            sl_LockObjUnlock (&wlan_LockObj);
        }
    }
    return RES_OK;
}

DRESULT sflash_disk_status(void) {
    if (!sflash_init_done) {
        return STA_NOINIT;
    }
    return RES_OK;
}

DRESULT sflash_disk_read(BYTE *buff, DWORD sector, UINT count) {
    uint32_t secindex;

    if (!sflash_init_done) {
        return STA_NOINIT;
    }

    if ((sector + count > SFLASH_SECTOR_COUNT) || (count == 0)) {
        return RES_PARERR;
    }

    for (int i = 0; i < count; i++) {
        secindex = (sector + i) % SFLASH_SECTORS_PER_BLOCK;
        sflash_ublock = (sector + i) / SFLASH_SECTORS_PER_BLOCK;
        // See if it's time to read a new block
        if (sflash_prblock != sflash_ublock) {
            if (sflash_disk_flush() != RES_OK) {
                return RES_ERROR;
            }
            sflash_prblock = sflash_ublock;
            print_block_name (sflash_ublock);
            if (!sflash_access(FS_MODE_OPEN_READ, sl_FsRead)) {
                return RES_ERROR;
            }
        }
        // Copy the requested sector from the block cache
        memcpy (buff, &sflash_block_cache[(secindex * SFLASH_SECTOR_SIZE)], SFLASH_SECTOR_SIZE);
        buff += SFLASH_SECTOR_SIZE;
    }
    return RES_OK;
}

DRESULT sflash_disk_write(const BYTE *buff, DWORD sector, UINT count) {
    uint32_t secindex;
    int32_t index = 0;

    if (!sflash_init_done) {
        return STA_NOINIT;
    }

    if ((sector + count > SFLASH_SECTOR_COUNT) || (count == 0)) {
        sflash_disk_flush();
        return RES_PARERR;
    }

    do {
        secindex = (sector + index) % SFLASH_SECTORS_PER_BLOCK;
        sflash_ublock = (sector + index) / SFLASH_SECTORS_PER_BLOCK;
        // Check if it's a different block than last time
        if (sflash_prblock != sflash_ublock) {
            if (sflash_disk_flush() != RES_OK) {
                return RES_ERROR;
            }
            sflash_prblock = sflash_ublock;
            print_block_name (sflash_ublock);
            // Read the block into the cache
            if (!sflash_access(FS_MODE_OPEN_READ, sl_FsRead)) {
                return RES_ERROR;
            }
        }
        // Copy the input sector to the block cache
        memcpy (&sflash_block_cache[(secindex * SFLASH_SECTOR_SIZE)], buff, SFLASH_SECTOR_SIZE);
        buff += SFLASH_SECTOR_SIZE;
        sflash_cache_is_dirty = true;
    } while (++index < count);

    return RES_OK;
}

DRESULT sflash_disk_flush (void) {
    // Write back the cache if it's dirty
    if (sflash_cache_is_dirty) {
        if (!sflash_access(FS_MODE_OPEN_WRITE, sl_FsWrite)) {
            return RES_ERROR;
        }
        sflash_cache_is_dirty = false;
    }
    return RES_OK;
}

