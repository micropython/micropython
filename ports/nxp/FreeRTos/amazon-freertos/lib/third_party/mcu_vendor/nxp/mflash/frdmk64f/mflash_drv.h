/*
 * Copyright 2017 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MFLASH_DRV_H__
#define __MFLASH_DRV_H__

#include <stdbool.h>
#include <stdint.h>

/* Flash constants */
#ifndef MFLASH_SECTOR_MASK
#define MFLASH_SECTOR_MASK (0xFFF)
#endif

#ifndef MFLASH_SECTOR_SIZE
#define MFLASH_SECTOR_SIZE (4096)
#endif

#ifndef MFLASH_PAGE_SIZE
#define MFLASH_PAGE_SIZE (256)
#endif

#ifndef MFLASH_BAUDRATE
#define MFLASH_BAUDRATE (96000000)
#endif

static inline uint32_t mflash_drv_is_sector_aligned(uint32_t addr)
{
    return ((addr) & (MFLASH_SECTOR_MASK)) == 0 ? true : false;
}

static inline uint32_t mflash_drv_addr_to_sector_addr(uint32_t addr)
{
    return (addr & (~((uint32_t)(MFLASH_SECTOR_MASK))));
}

static inline uint32_t mflash_drv_addr_to_sector_of(uint32_t addr)
{
    return ((addr) & ((uint32_t)(MFLASH_SECTOR_MASK)));
}

int32_t mflash_drv_init(void);
int32_t mflash_drv_write(void *any_addr, uint8_t *data, uint32_t data_len);

#endif
