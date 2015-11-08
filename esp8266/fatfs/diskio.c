/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * Original file from:
 * FatFs - Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Alex March
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

#include "py/runtime.h"
#include "lib/fatfs/ffconf.h"
#include "lib/fatfs/ff.h"       /* FatFs lower layer API */
#include "lib/fatfs/diskio.h"   /* FatFs lower layer API */
#include "flash.h"              /* Flash disk mount API */
#include "fsusermount.h"        /* User disk mount API */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv               /* Physical drive number to identify the drive */
)
{
    switch (pdrv) {
        case PD_FLASH:
            return flash_disk_status();

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            // TODO return STA_PROTECT if SD card is read only
            return RES_OK;
#endif

        case PD_USER:
            if (MP_STATE_PORT(fs_user_mount) == NULL) {
                return STA_NODISK;
            }
            if (MP_STATE_PORT(fs_user_mount)->writeblocks[0] == MP_OBJ_NULL) {
                return STA_PROTECT;
            }
            return RES_OK;
    }
    return STA_NODISK;
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive number to identify the drive */
)
{
    switch (pdrv) {
        case PD_FLASH:
            if (RES_OK != flash_disk_init()) {
                return STA_NOINIT;
            }
            return RES_OK;

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            if (!sdcard_power_on()) {
                return STA_NODISK;
            }
            // TODO return STA_PROTECT if SD card is read only
            return RES_OK;
#endif

        case PD_USER:
            if (MP_STATE_PORT(fs_user_mount) == NULL) {
                return STA_NODISK;
            }
            if (MP_STATE_PORT(fs_user_mount)->writeblocks[0] == MP_OBJ_NULL) {
                return STA_PROTECT;
            }
            return RES_OK;
    }
    return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,              /* Physical drive nmuber to identify the drive */
    BYTE *buff,             /* Data buffer to store read data */
    DWORD sector,           /* Sector address in LBA */
    UINT count              /* Number of sectors to read */
)
{
    switch (pdrv) {
        case PD_FLASH:
            return flash_disk_read(buff, sector, count);

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
    BYTE pdrv,              /* Physical drive nmuber to identify the drive */
    const BYTE *buff,       /* Data to be written */
    DWORD sector,           /* Sector address in LBA */
    UINT count              /* Number of sectors to write */
)
{
    switch (pdrv) {
        case PD_FLASH:
            return flash_disk_write(buff, sector, count);

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
    BYTE pdrv,              /* Physical drive nmuber (0..) */
    BYTE cmd,               /* Control code */
    void *buff              /* Buffer to send/receive control data */
)
{
    switch (pdrv) {
    case PD_FLASH:
        switch (cmd) {
            case CTRL_SYNC:
                return flash_disk_flush();
            case GET_SECTOR_COUNT:
                *((DWORD*)buff) = FLASH_SECTOR_COUNT;
                return RES_OK;
            case GET_SECTOR_SIZE:
                *((WORD*)buff) = FLASH_SECTOR_SIZE;
                return RES_OK;
            case GET_BLOCK_SIZE:
                *((DWORD*)buff) = 1; // sector erase size in units of the block
                return RES_OK;
        }
        break;

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

        case PD_USER:
            if (MP_STATE_PORT(fs_user_mount) == NULL) {
                // nothing mounted
                return RES_ERROR;
            }
            switch (cmd) {
                case CTRL_SYNC:
                    if (MP_STATE_PORT(fs_user_mount)->sync[0] != MP_OBJ_NULL) {
                        mp_call_method_n_kw(0, 0, MP_STATE_PORT(fs_user_mount)->sync);
                    }
                    return RES_OK;

                case GET_BLOCK_SIZE:
                    *((DWORD*)buff) = 1; // high-level sector erase size in units of the small (512) bl
                    return RES_OK;

                case GET_SECTOR_COUNT: {
                    mp_obj_t ret = mp_call_method_n_kw(0, 0, MP_STATE_PORT(fs_user_mount)->count);
                    *((DWORD*)buff) = mp_obj_get_int(ret);
                    return RES_OK;
                }
            }
            break;
    }
    return RES_PARERR;
}
#endif

#if !_FS_READONLY && !_FS_NORTC
DWORD get_fattime (
    void
)
{
    // need timeutils here if necessary
    return 0;
}
#endif