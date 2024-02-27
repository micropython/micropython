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
#include "py/mperrno.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs_fat.h"

typedef void *bdev_t;
static fs_user_mount_t *disk_get_device(void *bdev) {
    return (fs_user_mount_t *)bdev;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    bdev_t pdrv,      /* Physical drive nmuber (0..) */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,    /* Sector address (LBA) */
    UINT count        /* Number of sectors to read (1..128) */
    ) {
    fs_user_mount_t *vfs = disk_get_device(pdrv);
    if (vfs == NULL) {
        return RES_PARERR;
    }

    int ret = mp_vfs_blockdev_read(&vfs->blockdev, sector, count, buff);

    return ret == 0 ? RES_OK : RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write(
    bdev_t pdrv,          /* Physical drive nmuber (0..) */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Sector address (LBA) */
    UINT count            /* Number of sectors to write (1..128) */
    ) {
    fs_user_mount_t *vfs = disk_get_device(pdrv);
    if (vfs == NULL) {
        return RES_PARERR;
    }

    int ret = mp_vfs_blockdev_write(&vfs->blockdev, sector, count, buff);

    if (ret == -MP_EROFS) {
        // read-only block device
        return RES_WRPRT;
    }

    return ret == 0 ? RES_OK : RES_ERROR;
}


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
    bdev_t pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
    ) {
    fs_user_mount_t *vfs = disk_get_device(pdrv);
    if (vfs == NULL) {
        return RES_PARERR;
    }

    // First part: call the relevant method of the underlying block device
    static const uint8_t op_map[8] = {
        [CTRL_SYNC] = MP_BLOCKDEV_IOCTL_SYNC,
        [GET_SECTOR_COUNT] = MP_BLOCKDEV_IOCTL_BLOCK_COUNT,
        [GET_SECTOR_SIZE] = MP_BLOCKDEV_IOCTL_BLOCK_SIZE,
        [IOCTL_INIT] = MP_BLOCKDEV_IOCTL_INIT,
    };
    uint8_t bp_op = op_map[cmd & 7];
    mp_obj_t ret = mp_const_none;
    if (bp_op != 0) {
        ret = mp_vfs_blockdev_ioctl(&vfs->blockdev, bp_op, 0);
    }

    // Second part: convert the result for return
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT: {
            *((DWORD *)buff) = mp_obj_get_int(ret);
            return RES_OK;
        }

        case GET_SECTOR_SIZE: {
            if (ret == mp_const_none) {
                // Default sector size
                *((WORD *)buff) = 512;
            } else {
                *((WORD *)buff) = mp_obj_get_int(ret);
            }
            // need to store ssize because we use it in disk_read/disk_write
            vfs->blockdev.block_size = *((WORD *)buff);
            return RES_OK;
        }

        case GET_BLOCK_SIZE:
            *((DWORD *)buff) = 1; // erase block size in units of sector size
            return RES_OK;

        case IOCTL_INIT:
        case IOCTL_STATUS: {
            DSTATUS stat;
            if (ret != mp_const_none && MP_OBJ_SMALL_INT_VALUE(ret) != 0) {
                // error initialising
                stat = STA_NOINIT;
            } else if (vfs->blockdev.writeblocks[0] == MP_OBJ_NULL) {
                stat = STA_PROTECT;
            } else {
                stat = 0;
            }
            *((DSTATUS *)buff) = stat;
            return RES_OK;
        }

        default:
            return RES_PARERR;
    }
}

#endif // MICROPY_VFS && MICROPY_VFS_FAT
