/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/mpconfig.h"
#if MICROPY_VFS && MICROPY_VFS_FAT

#include <stdint.h>
#include <stdio.h>

#include "py/mphal.h"

#include "py/runtime.h"
#include "py/binary.h"
#include "py/objarray.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs_fat.h"

#if _MAX_SS == _MIN_SS
#define SECSIZE(fs) (_MIN_SS)
#else
#define SECSIZE(fs) ((fs)->ssize)
#endif

typedef void *bdev_t;
STATIC fs_user_mount_t *disk_get_device(void *bdev) {
    return (fs_user_mount_t*)bdev;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    bdev_t pdrv,      /* Physical drive nmuber (0..) */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,    /* Sector address (LBA) */
    UINT count        /* Number of sectors to read (1..128) */
)
{
    fs_user_mount_t *vfs = disk_get_device(pdrv);
    if (vfs == NULL) {
        return RES_PARERR;
    }

    if (vfs->flags & FSUSER_NATIVE) {
        mp_uint_t (*f)(uint8_t*, uint32_t, uint32_t) = (void*)(uintptr_t)vfs->readblocks[2];
        if (f(buff, sector, count) != 0) {
            return RES_ERROR;
        }
    } else {
        mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, count * SECSIZE(&vfs->fatfs), buff};
        vfs->readblocks[2] = MP_OBJ_NEW_SMALL_INT(sector);
        vfs->readblocks[3] = MP_OBJ_FROM_PTR(&ar);
        mp_call_method_n_kw(2, 0, vfs->readblocks);
        // TODO handle error return
    }

    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
    bdev_t pdrv,          /* Physical drive nmuber (0..) */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Sector address (LBA) */
    UINT count            /* Number of sectors to write (1..128) */
)
{
    fs_user_mount_t *vfs = disk_get_device(pdrv);
    if (vfs == NULL) {
        return RES_PARERR;
    }

    if (vfs->writeblocks[0] == MP_OBJ_NULL) {
        // read-only block device
        return RES_WRPRT;
    }

    if (vfs->flags & FSUSER_NATIVE) {
        mp_uint_t (*f)(const uint8_t*, uint32_t, uint32_t) = (void*)(uintptr_t)vfs->writeblocks[2];
        if (f(buff, sector, count) != 0) {
            return RES_ERROR;
        }
    } else {
        mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, count * SECSIZE(&vfs->fatfs), (void*)buff};
        vfs->writeblocks[2] = MP_OBJ_NEW_SMALL_INT(sector);
        vfs->writeblocks[3] = MP_OBJ_FROM_PTR(&ar);
        mp_call_method_n_kw(2, 0, vfs->writeblocks);
        // TODO handle error return
    }

    return RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    bdev_t pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    fs_user_mount_t *vfs = disk_get_device(pdrv);
    if (vfs == NULL) {
        return RES_PARERR;
    }

    switch (cmd) {
        case CTRL_SYNC:
            vfs->ioctl[2] = MP_OBJ_NEW_SMALL_INT(BP_IOCTL_SYNC);
            vfs->ioctl[3] = MP_OBJ_NEW_SMALL_INT(0); // unused
            mp_call_method_n_kw(2, 0, vfs->ioctl);
            return RES_OK;

        case GET_SECTOR_COUNT: {
            vfs->ioctl[2] = MP_OBJ_NEW_SMALL_INT(BP_IOCTL_SEC_COUNT);
            vfs->ioctl[3] = MP_OBJ_NEW_SMALL_INT(0); // unused
            mp_obj_t ret = mp_call_method_n_kw(2, 0, vfs->ioctl);
            *((DWORD*)buff) = mp_obj_get_int(ret);
            return RES_OK;
        }

        case GET_SECTOR_SIZE: {
            vfs->ioctl[2] = MP_OBJ_NEW_SMALL_INT(BP_IOCTL_SEC_SIZE);
            vfs->ioctl[3] = MP_OBJ_NEW_SMALL_INT(0); // unused
            mp_obj_t ret = mp_call_method_n_kw(2, 0, vfs->ioctl);
            if (ret == mp_const_none) {
                // Default sector size
                *((WORD*)buff) = 512;
            } else {
                *((WORD*)buff) = mp_obj_get_int(ret);
            }
            #if _MAX_SS != _MIN_SS
            // need to store ssize because we use it in disk_read/disk_write
            vfs->fatfs.ssize = *((WORD*)buff);
            #endif
            return RES_OK;
        }

        case GET_BLOCK_SIZE:
            *((DWORD*)buff) = 1; // erase block size in units of sector size
            return RES_OK;

        case IOCTL_INIT: {
            // call ioctl(INIT, 0)
            DSTATUS status;
            vfs->ioctl[2] = MP_OBJ_NEW_SMALL_INT(BP_IOCTL_INIT);
            vfs->ioctl[3] = MP_OBJ_NEW_SMALL_INT(0); // unused
            mp_obj_t ret = mp_call_method_n_kw(2, 0, vfs->ioctl);
            if (ret != mp_const_none && MP_OBJ_SMALL_INT_VALUE(ret) != 0) {
                // error initialising
                status = STA_NOINIT;
            } else if (vfs->writeblocks[0] == MP_OBJ_NULL) {
                status = STA_PROTECT;
            } else {
                status = 0;
            }
            *((DSTATUS*)buff) = status;
            return RES_OK;
        }

        case IOCTL_STATUS: {
            DSTATUS status;
            if (vfs->writeblocks[0] == MP_OBJ_NULL) {
                status = STA_PROTECT;
            } else {
                status = 0;
            }
            *((DSTATUS*)buff) = status;
            return RES_OK;
        }

        default:
            return RES_PARERR;
    }
}

#endif // MICROPY_VFS && MICROPY_VFS_FAT
