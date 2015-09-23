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

#include <stdint.h>
#include <stdbool.h>

#include "diskio.h"
#include "sflash_diskio.h"      /* Serial flash disk IO API */

/* Definitions of physical drive number for each drive */
#define SFLASH      0           /* Map SFLASH drive to drive number 0 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv               /* Physical drive number to identify the drive */
)
{
    switch (pdrv) {
    case SFLASH:
        return sflash_disk_status();
    default:
        break;
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
    DSTATUS stat = 0;

    switch (pdrv) {
    case SFLASH:
        if (RES_OK != sflash_disk_init()) {
            stat = STA_NOINIT;
        }
        return stat;
    default:
        break;
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
    case SFLASH:
        return sflash_disk_read(buff, sector, count);
    default:
        break;
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
    case SFLASH:
        return sflash_disk_write(buff, sector, count);
    default:
        break;
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
    case SFLASH:
        switch (cmd) {
        case CTRL_SYNC:
            return sflash_disk_flush();
        case GET_SECTOR_COUNT:
            // sector count for ESP8266
            // *((DWORD*)buff) = SFLASH_SECTOR_COUNT;
            return RES_OK;
        case GET_SECTOR_SIZE:
            // sector size for ESP8266
            // *((WORD*)buff) = SFLASH_SECTOR_SIZE;
            return RES_OK;
        case GET_BLOCK_SIZE:
            // *((DWORD*)buff) = 1; // high-level sector erase size in units of the block size
            return RES_OK;
        default:
            break;
        }
    default:
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
    // return timestamp
    return 0;
}
#endif