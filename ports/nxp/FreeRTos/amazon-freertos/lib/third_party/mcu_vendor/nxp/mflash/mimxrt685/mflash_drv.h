/*
 * Copyright 2017-2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MFLASH_DRV_H__
#define __MFLASH_DRV_H__

#include <stdbool.h>
#include <stdint.h>

/* Flash constants */
#ifndef MFLASH_SECTOR_SIZE
#define MFLASH_SECTOR_SIZE (4096U)
#endif

#ifndef MFLASH_SECTOR_MASK
#define MFLASH_SECTOR_MASK ((MFLASH_SECTOR_SIZE)-1)
#endif

#ifndef MFLASH_PAGE_SIZE
#define MFLASH_PAGE_SIZE (256U)
#endif

#ifndef MFLASH_PAGE_MASK
#define MFLASH_PAGE_MASK ((MFLASH_PAGE_SIZE)-1)
#endif

#define MFLASH_QSPI QUADSPI
#define QSPI_CLK_FREQ CLOCK_GetFreq(kCLOCK_QspiClk)
#define FLASH_SIZE 0x04000000U

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

void mflash_drv_init(void);
int32_t mflash_drv_write(void *sector_addr, uint8_t *a_data, uint32_t a_len, uint8_t *b_data, uint32_t b_len);

#endif
