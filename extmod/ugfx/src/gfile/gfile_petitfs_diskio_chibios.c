/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_PETITFS && GFX_USE_OS_CHIBIOS && !GFILE_PETITFS_EXTERNAL_LIB

#include "gfile_petitfs_wrapper.h"

#include <string.h>

#if HAL_USE_MMC_SPI && HAL_USE_SDC
#error "cannot specify both MMC_SPI and SDC drivers"
#endif

#if HAL_USE_MMC_SPI
extern MMCDriver MMCD1;
#elif HAL_USE_SDC
extern SDCDriver SDCD1;
#else
#error "MMC_SPI or SDC driver must be specified"
#endif

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */

DSTATUS disk_initialize (void) {
#if HAL_USE_MMC_SPI
    /* It is initialized externally, just reads the status.*/
    if (blkGetDriverState(&MMCD1) != BLK_READY)
    	return STA_NOINIT;
#else
    /* It is initialized externally, just reads the status.*/
    if (blkGetDriverState(&SDCD1) != BLK_READY)
    	return STA_NOINIT;
#endif
    // All good
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Read Part Sector(s)                                                        */

static BYTE		sectBuf[512];
static DWORD	sectpos = -1;

DRESULT disk_readp (
		BYTE* buff,    /* [OUT] Pointer to the read buffer */
		DWORD sector,  /* [IN]  Sector number */
		UINT offset,   /* [IN]  Byte offset in the sector to start to read */
		UINT count     /* [IN]  Number of bytes to read */
		) {

	if (sector != sectpos) {
		#if HAL_USE_MMC_SPI
			if (blkGetDriverState(&MMCD1) != BLK_READY)
				return RES_NOTRDY;
			if (mmcStartSequentialRead(&MMCD1, sector))
				return RES_ERROR;
			if (mmcSequentialRead(&MMCD1, sectBuf))
				return RES_ERROR;
			if (mmcStopSequentialRead(&MMCD1))
				return RES_ERROR;
		#else
			if (blkGetDriverState(&SDCD1) != BLK_READY)
				return RES_NOTRDY;
			if (sdcRead(&SDCD1, sector, sectBuf, 1))
				return RES_ERROR;
		#endif
			sectpos = sector;
	}
	memcpy(buff, sectBuf + offset, count);
	return RES_OK;
}

#endif // GFX_USE_GFILE && GFILE_NEED_PETITFS && GFX_USE_OS_CHIBIOS && !GFILE_PETITFS_EXTERNAL_LIB


