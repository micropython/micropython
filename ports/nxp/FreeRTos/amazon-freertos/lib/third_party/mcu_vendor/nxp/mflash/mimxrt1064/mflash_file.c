/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "mflash_file.h"
#include "mflash_drv.h"
#include "fsl_common.h"

static mflash_file_t *g_file_table = NULL;
static bool g_mflash_initialized = false;

/* Help to identify mfile without knowing an address */
#define MFLASH_META_MAGIC_NO (0xABECEDA8)

typedef struct {
    uint32_t file_size;
    uint32_t magic_no;
} mfile_meta_t;

bool mflash_is_initialized()
{
    return g_mflash_initialized;
}

BaseType_t mflash_init(mflash_file_t *user_file_table, bool init_drv)
{
    /* Check params */
    if (NULL == user_file_table)
        return pdFALSE;

    /* Check attributes of each file in table */
    for (
        mflash_file_t *tmp_file = user_file_table;
        (0 != tmp_file->flash_addr) && (0 != tmp_file->max_size);
        tmp_file += 1
    )
    {
        /* NOTE: This loop does not check overlapping sectors !!! */
        /* Assume that zero address and zero size belong to unitialized struct. */
        if ((0 == tmp_file->flash_addr) || (0 == tmp_file->max_size))
            return pdFALSE;
        /* Starting file address ('flash_addr') must be sector aligned */
        if (!mflash_drv_is_sector_aligned(tmp_file->flash_addr))
            return pdFALSE;
        /* File max_size must be multiple of sector size */
        if (!mflash_drv_is_sector_aligned(tmp_file->max_size))
            return pdFALSE;
    }
    /* Store reference to user*/
    g_file_table = user_file_table;
    /* Init flash driver */
    if (init_drv)
    {
        mflash_drv_init();
    }

    g_mflash_initialized = true;
    return pdTRUE;
}

/* Find a file in file_table */
int32_t mflash_find_file(char *path, mflash_file_t *file)
{
    int32_t status = -1;

    /* Check file_table */
    if (NULL == g_file_table)
        return -1;

    for (
        mflash_file_t *tmp_file = g_file_table;
        (0 != tmp_file->flash_addr) && (0 != tmp_file->max_size);
        tmp_file += 1
    )
    {
        /* Found a file by path */
        if (0 == strncmp(path, tmp_file->path, 64))
        {
            status = 0;
            *file = *tmp_file;
            break;
        }
    }
    return status;
}

/* NOTE: Don't try to store constant data that are located in XIP !! */
/* API, write to file of path 'pcFileName' */
BaseType_t mflash_save_file(
    char * pcFileName,
    uint8_t * pucData,
    uint32_t ulDataSize )
{
    mflash_file_t tmp_file = {0};
    mfile_meta_t tmp_meta = {0};

    /* No file was found in file table */
    if (0 != mflash_find_file(pcFileName, &tmp_file))
        return pdFALSE;
    /* Trying to write data over file boundary */
    if (ulDataSize > tmp_file.max_size + sizeof(tmp_meta))
        return pdFALSE;

    /* Erase sector for the new file - files are aligned with sectors */
    if (0 != mflash_drv_erase_sector((void *)tmp_file.flash_addr))
        return pdFALSE;

    /* Set meta data */
    tmp_meta.magic_no = MFLASH_META_MAGIC_NO;
    tmp_meta.file_size = ulDataSize;

    uint32_t page_buff[MFLASH_PAGE_SIZE/sizeof(uint32_t)];
    /* Prepare first page - start with meta data */
    memcpy(page_buff, &tmp_meta, sizeof(tmp_meta));

    uint32_t data_len = MFLASH_PAGE_SIZE - sizeof(tmp_meta);
    if (data_len > ulDataSize)
    {
        data_len = ulDataSize;
    }

    /* Fill the page buffer with data */
    memcpy((void*)((uint32_t)page_buff + sizeof(tmp_meta)), pucData, data_len);

    /* Write the page */
    if (0 != mflash_drv_write_page((void*)tmp_file.flash_addr, page_buff))
        return pdFALSE;

    uint32_t written = data_len;
    uint32_t page_idx = 1;
    while (written < ulDataSize)
    {
        data_len = ulDataSize - written;
        if (data_len > MFLASH_PAGE_SIZE)
        {
            data_len = MFLASH_PAGE_SIZE;
        }

        /* Fill page buffer with next data */
        memcpy(page_buff, pucData + written, data_len);

        /* Write the page */
        if (0 != mflash_drv_write_page((void *)(tmp_file.flash_addr + page_idx * MFLASH_PAGE_SIZE), page_buff))
            return pdFALSE;

        written += data_len;
        page_idx++;
    }

    return pdTRUE;
}

/* API, read file of path 'pcFileName' */
BaseType_t mflash_read_file(
    char * pcFileName,
    uint8_t ** ppucData,
    uint32_t * pulDataSize )
{
    mflash_file_t tmp_file = {0};
    mfile_meta_t *tmp_meta = NULL;
    /* No file was found in file table */
    if (0 != mflash_find_file(pcFileName, &tmp_file))
        return pdFALSE;
    /* Locate meta data and check magic number */
    tmp_meta = (mfile_meta_t*)tmp_file.flash_addr;
    /* To avoid hard-fault better check NULL ptr */
    if (NULL == tmp_meta)
            return pdFALSE;
    tmp_meta = (mfile_meta_t *)((uint32_t)tmp_meta + FlexSPI_AMBA_BASE);
    /* Magic number does not match, this location is not a valid file */
    if (MFLASH_META_MAGIC_NO != tmp_meta->magic_no)
            return pdFALSE;
    /* Set file size and set data pointer to real_data */
    *pulDataSize = tmp_meta->file_size;
    *ppucData = (uint8_t*)(tmp_file.flash_addr + sizeof(*tmp_meta) + FlexSPI_AMBA_BASE);

    return pdTRUE;
}
