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
#include "py/runtime.h"
#include "py/obj.h"
#include "lib/fatfs/ff.h"
#include "lib/fatfs/diskio.h"   /* FatFs lower layer API */
#include "sflash_diskio.h"      /* Serial flash disk IO API */
#include "sd_diskio.h"          /* SDCARD disk IO API */
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "prcm.h"
#include "pybrtc.h"
#include "timeutils.h"
#include "pybsd.h"
#include "moduos.h"


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    if (pdrv == PD_FLASH) {
        return sflash_disk_status();
    } else {
        os_fs_mount_t *mount_obj;
        if ((mount_obj = osmount_find_by_volume(pdrv))) {
            if (mount_obj->writeblocks[0] == MP_OBJ_NULL) {
                return STA_PROTECT;
            }
            return 0;
        }
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
    if (pdrv == PD_FLASH) {
        if (RES_OK != sflash_disk_init()) {
            return STA_NOINIT;
        }
    } else {
        os_fs_mount_t *mount_obj;
        if ((mount_obj = osmount_find_by_volume(pdrv))) {
            if (mount_obj->writeblocks[0] == MP_OBJ_NULL) {
                return STA_PROTECT;
            }
            return 0;
        }
    }
    return STA_NODISK;
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
    if (pdrv == PD_FLASH) {
        return sflash_disk_read(buff, sector, count);
    } else {
        os_fs_mount_t *mount_obj;
        if ((mount_obj = osmount_find_by_volume(pdrv))) {
            // optimization for the built-in sd card device
            if (mount_obj->device == (mp_obj_t)&pybsd_obj) {
                return sd_disk_read(buff, sector, count);
            }
            mount_obj->readblocks[2] = MP_OBJ_NEW_SMALL_INT(sector);
            mount_obj->readblocks[3] = mp_obj_new_bytearray_by_ref(count * 512, buff);
            return mp_obj_get_int(mp_call_method_n_kw(2, 0, mount_obj->readblocks));
        }
        // nothing mounted
        return RES_ERROR;
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
    if (pdrv == PD_FLASH) {
        return sflash_disk_write(buff, sector, count);
    } else {
        os_fs_mount_t *mount_obj;
        if ((mount_obj = osmount_find_by_volume(pdrv))) {
            // optimization for the built-in sd card device
            if (mount_obj->device == (mp_obj_t)&pybsd_obj) {
                return sd_disk_write(buff, sector, count);
            }
            mount_obj->writeblocks[2] = MP_OBJ_NEW_SMALL_INT(sector);
            mount_obj->writeblocks[3] = mp_obj_new_bytearray_by_ref(count * 512, (void *)buff);
            return mp_obj_get_int(mp_call_method_n_kw(2, 0, mount_obj->writeblocks));
        }
        // nothing mounted
        return RES_ERROR;
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
    if (pdrv == PD_FLASH) {
        switch (cmd) {
        case CTRL_SYNC:
            return sflash_disk_flush();
        case GET_SECTOR_COUNT:
            *((DWORD*)buff) = SFLASH_SECTOR_COUNT;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((DWORD*)buff) = SFLASH_SECTOR_SIZE;
            return RES_OK;
        case GET_BLOCK_SIZE:
            *((DWORD*)buff) = 1; // high-level sector erase size in units of the block size
            return RES_OK;
        }
    } else {
        os_fs_mount_t *mount_obj;
        if ((mount_obj = osmount_find_by_volume(pdrv))) {
            switch (cmd) {
            case CTRL_SYNC:
                if (mount_obj->sync[0] != MP_OBJ_NULL) {
                    mp_call_method_n_kw(0, 0, mount_obj->sync);
                }
                return RES_OK;
            case GET_SECTOR_COUNT:
                // optimization for the built-in sd card device
                if (mount_obj->device == (mp_obj_t)&pybsd_obj) {
                    *((DWORD*)buff) = sd_disk_info.ulNofBlock * (sd_disk_info.ulBlockSize / 512);
                } else {
                    *((DWORD*)buff) = mp_obj_get_int(mp_call_method_n_kw(0, 0, mount_obj->count));
                }
                return RES_OK;
            case GET_SECTOR_SIZE:
                *((DWORD*)buff) = SD_SECTOR_SIZE;  // Sector size is fixed to 512 bytes, as with SD cards
                return RES_OK;
            case GET_BLOCK_SIZE:
                *((DWORD*)buff) = 1; // high-level sector erase size in units of the block size
                return RES_OK;
            }
        }
        // nothing mounted
        return RES_ERROR;
    }
    return RES_PARERR;
}
#endif

#if !_FS_READONLY && !_FS_NORTC
DWORD get_fattime (
    void
)
{
    timeutils_struct_time_t tm;
    timeutils_seconds_since_2000_to_struct_time(pyb_rtc_get_seconds(), &tm);

    return ((tm.tm_year - 1980) << 25) | ((tm.tm_mon) << 21)  |
            ((tm.tm_mday) << 16)       | ((tm.tm_hour) << 11) |
            ((tm.tm_min) << 5)         | (tm.tm_sec >> 1);
}
#endif

