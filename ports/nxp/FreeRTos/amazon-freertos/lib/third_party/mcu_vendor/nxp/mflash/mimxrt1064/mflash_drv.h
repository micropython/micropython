/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MFLASH_DRV_H__
#define __MFLASH_DRV_H__

#include <stdbool.h>
#include <stdint.h>

/* Flash constants */
#ifndef MFLASH_SECTOR_SIZE
#define MFLASH_SECTOR_SIZE  (0x1000)
#endif

#ifndef MFLASH_PAGE_SIZE
#define MFLASH_PAGE_SIZE    (256)
#endif

#ifndef MFLASH_FLEXSPI
#define MFLASH_FLEXSPI      FLEXSPI
#endif


static inline uint32_t mflash_drv_is_page_aligned(uint32_t addr)
{
    return ((addr) & (MFLASH_PAGE_SIZE - 1)) == 0 ? true : false;
}

static inline uint32_t mflash_drv_is_sector_aligned(uint32_t addr)
{
    return ((addr) & (MFLASH_SECTOR_SIZE - 1)) == 0 ? true : false;
}


int32_t mflash_drv_init(void);
int32_t mflash_drv_write_page(void *page_addr, uint32_t *data);
int32_t mflash_drv_erase_sector(void *sector_addr);

#endif
