/*
 * Copyright 2017-2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MFLASH_DRV_H__
#define __MFLASH_DRV_H__

#include <stdbool.h>
#include <stdint.h>


/* Power 2 of n
 * The value is not real sector size reported by FLASH_GetProperty() in mflash_drv_init().
 * The sector size is decreased so it is possible to store more files in mflash file system,
 * this platform can erase/program the flash memory by smaller block than is the real sector size.
 */
#ifndef MFLASH_SECTOR_SIZE
#define MFLASH_SECTOR_SIZE  (0x1000)//(0x8000)
#endif

/* Flash constants */
#ifndef MFLASH_SECTOR_MASK
#define MFLASH_SECTOR_MASK  ((MFLASH_SECTOR_SIZE) - 1)
#endif

#ifndef MFLASH_WORD_SIZE
#define MFLASH_WORD_SIZE (512)
#endif

#ifndef MFLASH_WORD_MASK
#define MFLASH_WORD_MASK ((MFLASH_WORD_SIZE) - 1)
#endif

static inline uint32_t mflash_drv_is_sector_aligned(uint32_t addr)
{
    return ((addr) & (MFLASH_SECTOR_MASK)) == 0 ? true : false;
}


int32_t mflash_drv_init(void);
int32_t mflash_drv_is_readable(void *any_addr);
int32_t mflash_drv_write(void *any_addr, uint8_t *a_data, uint32_t a_len, uint8_t *b_data, uint32_t b_len);

#endif
