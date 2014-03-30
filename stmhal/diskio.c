/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include "ff.h"        /* FatFs lower layer API */
#include "diskio.h"        /* FatFs lower layer API */
#include "misc.h"
#include "storage.h"
#include "sdcard.h"

PARTITION VolToPart[] = {
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
            if (!sdcard_read_blocks(buff, sector, count)) {
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
            if (!sdcard_write_blocks(buff, sector, count)) {
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
    // TODO replace with call to RTC
    int year = 2013;
    int month = 10;
    int day = 12;
    int hour = 21;
    int minute = 42;
    int second = 13;
    return ((year - 1980) << 25) | ((month) << 21) | ((day) << 16) | ((hour) << 11) | ((minute) << 5) | (second / 2);
}
