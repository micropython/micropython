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

#include "stm32f4xx_hal.h"

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "systick.h"
#include "rtc.h"
#include "storage.h"
#include "sdcard.h"
#include "ff.h"        /* FatFs lower layer API */
#include "diskio.h"        /* FatFs lower layer API */

const PARTITION VolToPart[] = {
    {0, 1},     // Logical drive 0 ==> Physical drive 0, 1st partition
    {1, 0},     // Logical drive 1 ==> Physical drive 1 (auto detection)
    /*
    {0, 2},     // Logical drive 2 ==> Physical drive 0, 2nd partition
    {0, 3},     // Logical drive 3 ==> Physical drive 0, 3rd partition
    */
};

/* Definitions of physical drive number for each media */
#define PD_FLASH (0)
#define PD_SDCARD (1)

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv                /* Physical drive nmuber (0..) */
)
{
    switch (pdrv) {
        case PD_FLASH:
            storage_init();
            return 0;

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            if (!sdcard_power_on()) {
                return STA_NODISK;
            }
            // TODO return STA_PROTECT if SD card is read only
            return 0;
#endif
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
        case PD_FLASH:
            for (int i = 0; i < count; i++) {
                if (!storage_read_block(buff + i * FLASH_BLOCK_SIZE, sector + i)) {
                    return RES_ERROR;
                }
            }
            return RES_OK;

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            if (sdcard_read_blocks(buff, sector, count) != 0) {
                return RES_ERROR;
            }
            return RES_OK;
#endif
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
        case PD_FLASH:
            for (int i = 0; i < count; i++) {
                if (!storage_write_block(buff + i * FLASH_BLOCK_SIZE, sector + i)) {
                    return RES_ERROR;
                }
            }
            return RES_OK;

#if MICROPY_HW_HAS_SDCARD
        case PD_SDCARD:
            if (sdcard_write_blocks(buff, sector, count) != 0) {
                return RES_ERROR;
            }
            return RES_OK;
#endif
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
    }

    return RES_PARERR;
}
#endif

DWORD get_fattime (
    void
)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    HAL_RTC_GetTime(&RTCHandle, &time, FORMAT_BIN);
    HAL_RTC_GetDate(&RTCHandle, &date, FORMAT_BIN);
    return ((2000 + date.Year - 1980) << 25) | ((date.Month) << 21) | ((date.Date) << 16) | ((time.Hours) << 11) | ((time.Minutes) << 5) | (time.Seconds / 2);
}
