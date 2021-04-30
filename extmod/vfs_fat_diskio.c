// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

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

#if FF_MAX_SS == FF_MIN_SS
#define SECSIZE(fs) (FF_MIN_SS)
#else
#define SECSIZE(fs) ((fs)->ssize)
#endif

typedef void *bdev_t;
STATIC fs_user_mount_t *disk_get_device(void *bdev) {
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

    if (vfs->flags & FSUSER_NATIVE) {
        mp_uint_t (*f)(uint8_t *, uint32_t, uint32_t) = (void *)(uintptr_t)vfs->readblocks[2];
        if (f(buff, sector, count) != 0) {
            return RES_ERROR;
        }
    } else {
        mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, count *SECSIZE(&vfs->fatfs), buff};
        vfs->readblocks[2] = MP_OBJ_NEW_SMALL_INT(sector);
        vfs->readblocks[3] = MP_OBJ_FROM_PTR(&ar);
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t ret = mp_call_method_n_kw(2, 0, vfs->readblocks);
            nlr_pop();
            if (ret != mp_const_none && MP_OBJ_SMALL_INT_VALUE(ret) != 0) {
                return RES_ERROR;
            }
        } else {
            // Exception thrown by readblocks or something it calls.
            return RES_ERROR;
        }
    }

    return RES_OK;
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

    if (vfs->writeblocks[0] == MP_OBJ_NULL) {
        // read-only block device
        return RES_WRPRT;
    }

    if (vfs->flags & FSUSER_NATIVE) {
        mp_uint_t (*f)(const uint8_t *, uint32_t, uint32_t) = (void *)(uintptr_t)vfs->writeblocks[2];
        if (f(buff, sector, count) != 0) {
            return RES_ERROR;
        }
    } else {
        mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, count *SECSIZE(&vfs->fatfs), (void *)buff};
        vfs->writeblocks[2] = MP_OBJ_NEW_SMALL_INT(sector);
        vfs->writeblocks[3] = MP_OBJ_FROM_PTR(&ar);
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t ret = mp_call_method_n_kw(2, 0, vfs->writeblocks);
            nlr_pop();
            if (ret != mp_const_none && MP_OBJ_SMALL_INT_VALUE(ret) != 0) {
                return RES_ERROR;
            }
        } else {
            // Exception thrown by writeblocks or something it calls.
            return RES_ERROR;
        }
    }

    return RES_OK;
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
    mp_int_t out_value = 0;
    if (vfs->flags & FSUSER_HAVE_IOCTL) {
        // new protocol with ioctl
        static const uint8_t op_map[8] = {
            [CTRL_SYNC] = BP_IOCTL_SYNC,
            [GET_SECTOR_COUNT] = BP_IOCTL_SEC_COUNT,
            [GET_SECTOR_SIZE] = BP_IOCTL_SEC_SIZE,
            [IOCTL_INIT] = BP_IOCTL_INIT,
        };
        uint8_t bp_op = op_map[cmd & 7];
        if (bp_op != 0) {
            if (vfs->flags & FSUSER_NATIVE) {
                bool (*f)(size_t, mp_int_t *) = (void *)(uintptr_t)vfs->u.ioctl[2];
                if (!f(bp_op, (mp_int_t *)&out_value)) {
                    return RES_ERROR;
                }
            } else {
                vfs->u.ioctl[2] = MP_OBJ_NEW_SMALL_INT(bp_op);
                vfs->u.ioctl[3] = MP_OBJ_NEW_SMALL_INT(0); // unused
                mp_obj_t ret = mp_call_method_n_kw(2, 0, vfs->u.ioctl);
                if (ret != mp_const_none) {
                    out_value = mp_obj_get_int(ret);
                }
            }
        }
    } else {
        // old protocol with sync and count
        switch (cmd) {
            case CTRL_SYNC:
                if (vfs->u.old.sync[0] != MP_OBJ_NULL) {
                    mp_call_method_n_kw(0, 0, vfs->u.old.sync);
                }
                break;

            case GET_SECTOR_COUNT: {
                mp_obj_t ret = mp_call_method_n_kw(0, 0, vfs->u.old.count);
                if (ret != mp_const_none) {
                    out_value = mp_obj_get_int(ret);
                }
                break;
            }
            case GET_SECTOR_SIZE:
                // old protocol has fixed sector size of 512 bytes
                break;

            case IOCTL_INIT:
                // old protocol doesn't have init
                break;
        }
    }

    // Second part: convert the result for return
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT: {
            *((DWORD *)buff) = out_value;
            return RES_OK;
        }

        case GET_SECTOR_SIZE: {
            if (out_value == 0) {
                // Default sector size
                *((WORD *)buff) = 512;
            } else {
                *((WORD *)buff) = out_value;
            }
            #if FF_MAX_SS != FF_MIN_SS
            // need to store ssize because we use it in disk_read/disk_write
            vfs->fatfs.ssize = *((WORD *)buff);
            #endif
            return RES_OK;
        }

        case GET_BLOCK_SIZE:
            *((DWORD *)buff) = 1; // erase block size in units of sector size
            return RES_OK;

        case IOCTL_INIT:
        case IOCTL_STATUS: {
            DSTATUS stat;
            if (out_value != 0) {
                // error initialising
                stat = STA_NOINIT;
            } else if (vfs->writeblocks[0] == MP_OBJ_NULL) {
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
