/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * Original template for this file comes from:
 * Low level disk I/O module skeleton for FatFs, (C)ChaN, 2013
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

#include <stdint.h>
#include <stdio.h>

#include "py/mphal.h"

#include "py/runtime.h"
#include "lib/fatfs/ff.h"        /* FatFs lower layer API */
#include "lib/fatfs/diskio.h"    /* FatFs lower layer API */
#include "storage.h"
#include "sdcard.h"
#include "extmod/fsusermount.h"

// constants for block protocol ioctl
//#define BP_IOCTL_INIT           (1) // unused
//#define BP_IOCTL_DEINIT         (2) // unused
#define BP_IOCTL_SYNC           (3)
#define BP_IOCTL_SEC_COUNT      (4)
#define BP_IOCTL_SEC_SIZE       (5)

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv                /* Physical drive nmuber (0..) */
)
{
    switch (pdrv) {
#if MICROPY_HW_HAS_FLASH
        case PD_FLASH:
            storage_init();
            return 0;
#endif

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            if (!sdcard_power_on()) {
                return STA_NODISK;
            }
            // TODO return STA_PROTECT if SD card is read only
            return 0;
#endif

        case PD_USER:
            if (MP_STATE_PORT(fs_user_mount) == NULL) {
                return STA_NODISK;
            }
            if (MP_STATE_PORT(fs_user_mount)->writeblocks[0] == MP_OBJ_NULL) {
                return STA_PROTECT;
            }
            return 0;
    }

    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv        /* Physical drive nmuber (0..) */
)
{
    switch (pdrv) {
        case PD_FLASH :
            // flash is ready
            return 0;

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            // TODO return STA_PROTECT if SD card is read only
            return 0;
#endif

        case PD_USER:
            if (MP_STATE_PORT(fs_user_mount) == NULL) {
                return STA_NODISK;
            }
            if (MP_STATE_PORT(fs_user_mount)->writeblocks[0] == MP_OBJ_NULL) {
                return STA_PROTECT;
            }
            return 0;
    }

    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,        /* Physical drive nmuber (0..) */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,    /* Sector address (LBA) */
    UINT count        /* Number of sectors to read (1..128) */
)
{
    switch (pdrv) {
#if MICROPY_HW_HAS_FLASH
        case PD_FLASH:
            for (int i = 0; i < count; i++) {
                if (!storage_read_block(buff + i * FLASH_BLOCK_SIZE, sector + i)) {
                    return RES_ERROR;
                }
            }
            return RES_OK;
#endif

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            if (sdcard_read_blocks(buff, sector, count) != 0) {
                return RES_ERROR;
            }
            return RES_OK;
#endif

        case PD_USER:
            if (MP_STATE_PORT(fs_user_mount) == NULL) {
                // nothing mounted
                return RES_ERROR;
            }
            MP_STATE_PORT(fs_user_mount)->readblocks[2] = MP_OBJ_NEW_SMALL_INT(sector);
            MP_STATE_PORT(fs_user_mount)->readblocks[3] = mp_obj_new_bytearray_by_ref(count * 512, buff);
            mp_call_method_n_kw(2, 0, MP_STATE_PORT(fs_user_mount)->readblocks);
            return RES_OK;
    }

    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
    BYTE pdrv,            /* Physical drive nmuber (0..) */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Sector address (LBA) */
    UINT count            /* Number of sectors to write (1..128) */
)
{
    switch (pdrv) {
#if MICROPY_HW_HAS_FLASH
        case PD_FLASH:
            for (int i = 0; i < count; i++) {
                if (!storage_write_block(buff + i * FLASH_BLOCK_SIZE, sector + i)) {
                    return RES_ERROR;
                }
            }
            return RES_OK;
#endif

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            if (sdcard_write_blocks(buff, sector, count) != 0) {
                return RES_ERROR;
            }
            return RES_OK;
#endif

        case PD_USER:
            if (MP_STATE_PORT(fs_user_mount) == NULL) {
                // nothing mounted
                return RES_ERROR;
            }
            if (MP_STATE_PORT(fs_user_mount)->writeblocks[0] == MP_OBJ_NULL) {
                // read-only block device
                return RES_ERROR;
            }
            MP_STATE_PORT(fs_user_mount)->writeblocks[2] = MP_OBJ_NEW_SMALL_INT(sector);
            MP_STATE_PORT(fs_user_mount)->writeblocks[3] = mp_obj_new_bytearray_by_ref(count * 512, (void*)buff);
            mp_call_method_n_kw(2, 0, MP_STATE_PORT(fs_user_mount)->writeblocks);
            return RES_OK;
    }

    return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
    BYTE pdrv,        /* Physical drive nmuber (0..) */
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    switch (pdrv) {
#if MICROPY_HW_HAS_FLASH
        case PD_FLASH:
            switch (cmd) {
                case CTRL_SYNC:
                    storage_flush();
                    return RES_OK;

                case GET_BLOCK_SIZE:
                    *((DWORD*)buff) = 1; // high-level sector erase size in units of the small (512) block size
                    return RES_OK;
            }
            break;
#endif

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            switch (cmd) {
                case CTRL_SYNC:
                    return RES_OK;

                case GET_BLOCK_SIZE:
                    *((DWORD*)buff) = 1; // high-level sector erase size in units of the small (512) block size
                    return RES_OK;
            }
            break;
#endif

        case PD_USER: {
            fs_user_mount_t *vfs = MP_STATE_PORT(fs_user_mount);
            if (vfs == NULL) {
                // nothing mounted
                return RES_ERROR;
            }
            if (vfs->u.old.count[1] == MP_OBJ_SENTINEL) {
                // new protocol with ioctl
                switch (cmd) {
                    case CTRL_SYNC:
                        vfs->u.ioctl[2] = MP_OBJ_NEW_SMALL_INT(BP_IOCTL_SYNC);
                        vfs->u.ioctl[3] = MP_OBJ_NEW_SMALL_INT(0); // unused
                        mp_call_method_n_kw(2, 0, vfs->u.ioctl);
                        vfs->u.ioctl[3] = MP_OBJ_SENTINEL; // indicate new protocol
                        return RES_OK;

                    case GET_SECTOR_COUNT: {
                        vfs->u.ioctl[2] = MP_OBJ_NEW_SMALL_INT(BP_IOCTL_SEC_COUNT);
                        vfs->u.ioctl[3] = MP_OBJ_NEW_SMALL_INT(0); // unused
                        mp_obj_t ret = mp_call_method_n_kw(2, 0, vfs->u.ioctl);
                        *((DWORD*)buff) = mp_obj_get_int(ret);
                        vfs->u.ioctl[3] = MP_OBJ_SENTINEL; // indicate new protocol
                        return RES_OK;
                    }

                    case GET_SECTOR_SIZE: {
                        vfs->u.ioctl[2] = MP_OBJ_NEW_SMALL_INT(BP_IOCTL_SEC_SIZE);
                        vfs->u.ioctl[3] = MP_OBJ_NEW_SMALL_INT(0); // unused
                        mp_obj_t ret = mp_call_method_n_kw(2, 0, vfs->u.ioctl);
                        *((WORD*)buff) = mp_obj_get_int(ret);
                        vfs->u.ioctl[3] = MP_OBJ_SENTINEL; // indicate new protocol
                        return RES_OK;
                    }

                    case GET_BLOCK_SIZE:
                        *((DWORD*)buff) = 1; // erase block size in units of sector size
                        return RES_OK;
                }
            } else {
                // old protocol with sync and count
                switch (cmd) {
                    case CTRL_SYNC:
                        if (vfs->u.old.sync[0] != MP_OBJ_NULL) {
                            mp_call_method_n_kw(0, 0, vfs->u.old.sync);
                        }
                        return RES_OK;

                    case GET_SECTOR_COUNT: {
                        mp_obj_t ret = mp_call_method_n_kw(0, 0, vfs->u.old.count);
                        *((DWORD*)buff) = mp_obj_get_int(ret);
                        return RES_OK;
                    }

                    case GET_SECTOR_SIZE:
                        *((WORD*)buff) = 512; // old protocol had fixed sector size
                        return RES_OK;

                    case GET_BLOCK_SIZE:
                        *((DWORD*)buff) = 1; // erase block size in units of sector size
                        return RES_OK;
                }
            }
            break;
        }
    }

    return RES_PARERR;
}
#endif
