/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#include "py/mpconfig.h"
#include "diskio.h"		        /* FatFs lower layer API */
#include "sflash_diskio.h"      /* Serial flash disk IO API */
#if MICROPY_HW_HAS_SDCARD
#include "sd_diskio.h"		    /* SDCARD disk IO API */
#endif
#include "modutime.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"

/* Definitions of physical drive number for each drive */
#define SFLASH		0	/* Map SFLASH drive to drive number 0 */
#define SDCARD	    1	/* Map SD card to drive number 1 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
	case SFLASH :
	    return sflash_disk_status();
#if MICROPY_HW_HAS_SDCARD
	case SDCARD :
	    return sd_disk_status();
#endif
	default:
	    break;
	}
	return STA_NODISK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = 0;

	switch (pdrv) {
	case SFLASH :
		if (RES_OK != sflash_disk_init()) {
		    stat = STA_NOINIT;
		}
		return stat;
#if MICROPY_HW_HAS_SDCARD
    case SDCARD :
        if (RES_OK != sd_disk_init()) {
            stat = STA_NOINIT;
        }
        return stat;
#endif
    default:
        break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	switch (pdrv) {
	case SFLASH :
	    return sflash_disk_read(buff, sector, count);
#if MICROPY_HW_HAS_SDCARD
    case SDCARD :
        return sd_disk_read(buff, sector, count);
#endif
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
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	switch (pdrv) {
	case SFLASH :
		return sflash_disk_write(buff, sector, count);
#if MICROPY_HW_HAS_SDCARD
    case SDCARD :
        return sd_disk_write(buff, sector, count);
#endif
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
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    switch (pdrv) {
    case SFLASH:
        switch (cmd) {
        case CTRL_SYNC:
            return sflash_disk_flush();
        case GET_SECTOR_COUNT:
            *((DWORD*)buff) = SFLASH_SECTOR_COUNT;
            return RES_OK;
            break;
        case GET_SECTOR_SIZE:
            *((WORD*)buff) = SFLASH_SECTOR_SIZE;
            return RES_OK;
            break;
        case GET_BLOCK_SIZE:
            *((DWORD*)buff) = 1; // high-level sector erase size in units of the block size
            return RES_OK;
        }
        break;
#if MICROPY_HW_HAS_SDCARD
    case SDCARD:
        switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *(WORD*)buff = sd_disk_info.ulNofBlock;
            break;
        case GET_SECTOR_SIZE :
            *(WORD*)buff = SD_SECTOR_SIZE;
            break;
        case GET_BLOCK_SIZE:
            *((DWORD*)buff) = 1; // high-level sector erase size in units of the block size
            return RES_OK;
        }
        break;
#endif
    }
    return RES_PARERR;
}
#endif

#if !_FS_READONLY && !_FS_NORTC
DWORD get_fattime (
    void
)
{
    mod_struct_time tm;
    uint32_t seconds;
    uint16_t mseconds;

    // Get the time from the on-chip RTC and convert it to struct_time
    MAP_PRCMRTCGet(&seconds, &mseconds);
    mod_time_seconds_since_2000_to_struct_time(seconds, &tm);

    return ((tm.tm_year - 1980) << 25) | ((tm.tm_mon) << 21)  |
            ((tm.tm_mday) << 16)       | ((tm.tm_hour) << 11) |
            ((tm.tm_min) << 5)         | (tm.tm_sec >> 1);
}
#endif

