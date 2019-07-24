/*
 * Copyright 2017-2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MFLASH_FILE__
#define __MFLASH_FILE__

#include "mflash_drv.h"

typedef struct
{
    uint32_t flash_addr;
    uint32_t max_size;
    char path[64];
} mflash_file_t;

#define MFLASH_FILE_BASEADDR (0x000f0000)
#define MFLASH_FILE_SIZE (MFLASH_SECTOR_SIZE)

bool mflash_is_initialized(void);

BaseType_t mflash_init(mflash_file_t *user_file_table, bool init_drv);

BaseType_t mflash_read_file(char *pcFileName, uint8_t **ppucData, uint32_t *pulDataSize);

BaseType_t mflash_save_file(char *pcFileName, uint8_t *pucData, uint32_t ulDataSize);

#endif
