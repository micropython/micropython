/*
 * Copyright 2017-2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_iap.h"
#include "mflash_drv.h"
#include "pin_mux.h"
#include <stdbool.h>

static uint8_t g_tmp_data[(MFLASH_WORD_SIZE)];
static flash_config_t g_flash_instance = {0};
static uint32_t g_pflashBlockBase = 0;
static uint32_t g_pflashTotalSize = 0;
static uint32_t g_pflashSectorSize = 0;
static uint32_t g_pflashPageSize = 0;

/* API - initialize 'mflash' */
int32_t mflash_drv_init(void)
{
    volatile int32_t result = 0;

    if (FLASH_Init(&g_flash_instance) != kStatus_Success)
        return -1;

    if (FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashBlockBaseAddr, &g_pflashBlockBase)  != kStatus_Success)
        return -1;

    if (FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashSectorSize, &g_pflashSectorSize))
        return -1;

/* Disable this check, because the MFLASH_SECTOR_SIZE was decreased so it is possible to store more files in mflash files system. */
//    if (MFLASH_SECTOR_SIZE != g_pflashSectorSize)
//        return -1;

    if (FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashTotalSize, &g_pflashTotalSize))
        return -1;

    if (FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashPageSize, &g_pflashPageSize))
        return -1;

    return result;
}

/* Calling wrapper for 'mflash_drv_write_internal'.
 * Write 'data' of 'data_len' to 'any_addr' - which doesn't have to be sector aligned.
 * NOTE: Don't try to store constant data that are located in XIP !!
 */
int32_t mflash_drv_write(void *any_addr, uint8_t *a_data, uint32_t a_len, uint8_t *b_data, uint32_t b_len)
{
    uint32_t total_len, ceiling_len;
    uint32_t block_i, block_max;
    uint32_t tmp_a_len = a_len, tmp_b_len = b_len;
    uint8_t *tmp_a_data = a_data, *tmp_b_data = b_data;
    uint32_t tmp_addr = (uint32_t)any_addr;

    total_len = a_len + b_len;
    ceiling_len = (total_len + (MFLASH_WORD_MASK)) & (~((uint32_t)(MFLASH_WORD_MASK)));
    block_max = ceiling_len / (MFLASH_WORD_SIZE);

    if (FLASH_Erase(&g_flash_instance, tmp_addr, ceiling_len, kFLASH_ApiEraseKey) != kStatus_Success)
        return -1;

    for (block_i = 0; block_i < block_max; block_i++)
    {
        /* Process word-aligned a_data */
        if (tmp_a_len >= (MFLASH_WORD_SIZE))
        {
            if (FLASH_Program(&g_flash_instance, tmp_addr, tmp_a_data, (MFLASH_WORD_SIZE)) != kStatus_Success)
                return -1;
            tmp_a_data += (MFLASH_WORD_SIZE);
            tmp_a_len -= (MFLASH_WORD_SIZE);
        }
        /* Process word-aligned b_data  */
        else if (tmp_a_len == 0 && tmp_b_len >= (MFLASH_WORD_SIZE))
        {
            if (FLASH_Program(&g_flash_instance, tmp_addr, tmp_b_data, (MFLASH_WORD_SIZE)) != kStatus_Success)
                return -1;
            tmp_b_data += (MFLASH_WORD_SIZE);
            tmp_b_len -= (MFLASH_WORD_SIZE);
        }
        /* Process word-unaligned a/b_data  */
        else if (
           (tmp_a_len && tmp_a_len < (MFLASH_WORD_SIZE)) || 
           (tmp_b_len && tmp_b_len < (MFLASH_WORD_SIZE))
        )
        {            
            uint32_t tmp_i = 0, tmp_len = 0;
            /* Trailing a_data */
            if (tmp_a_len)
            {
                memcpy(&g_tmp_data[tmp_i], tmp_a_data, tmp_a_len);
                tmp_i = tmp_a_len;
                tmp_a_len = 0;
            }
            /* Leading or trailing b_data */
            if (tmp_b_len)
            {
                tmp_len = (MFLASH_WORD_SIZE) - tmp_i;
                tmp_len = tmp_len > tmp_b_len ? tmp_b_len : tmp_len;
                memcpy(&g_tmp_data[tmp_i], tmp_b_data, tmp_len);
                tmp_b_data += tmp_len;
                tmp_b_len -= tmp_len;
            }
            if (FLASH_Program(&g_flash_instance, tmp_addr, g_tmp_data, (MFLASH_WORD_SIZE)) != kStatus_Success)
                return -1;
        }
        else
        {
            /* should not happen */
            assert(0);
            return -1;
        }
        tmp_addr += (MFLASH_WORD_SIZE);
    }

    return 0;
}


int32_t mflash_drv_is_readable(void *any_addr)
{
#define FLASH_READMODE_ECC_MASK            (0x4U)
#define FLASH_READMODE_ECC_SHIFT           (2U)
#define FLASH_READMODE_ECC(x)              (((uint32_t)(((uint32_t)(x)) << FLASH_READMODE_ECC_SHIFT)) & FLASH_READMODE_ECC_MASK)
#define FLASH_READMODE_MARGIN_MASK         (0xC00U)
#define FLASH_READMODE_MARGIN_SHIFT        (10U)
#define FLASH_READMODE_MARGIN(x)           (((uint32_t)(((uint32_t)(x)) << FLASH_READMODE_MARGIN_SHIFT)) & FLASH_READMODE_MARGIN_MASK)
#define FLASH_READMODE_DMACC_MASK          (0x8000U)
#define FLASH_READMODE_DMACC_SHIFT         (15U)
#define FLASH_READMODE_DMACC(x)            (((uint32_t)(((uint32_t)(x)) << FLASH_READMODE_DMACC_SHIFT)) & FLASH_READMODE_DMACC_MASK)

    uint32_t addr = (uint32_t)any_addr;
    uint32_t result = 0, result_mask = 0;

    FLASH->INT_CLR_STATUS = FLASH_INT_CLR_STATUS_FAIL_MASK | FLASH_INT_CLR_STATUS_ERR_MASK |
                            FLASH_INT_CLR_STATUS_DONE_MASK | FLASH_INT_CLR_STATUS_ECC_ERR_MASK;
    FLASH->STARTA = addr >> 4;
    FLASH->DATAW[0] = FLASH_READMODE_ECC(g_flash_instance.modeConfig.readSingleWord.readWithEccOff) |
                         FLASH_READMODE_MARGIN(g_flash_instance.modeConfig.readSingleWord.readMarginLevel) |
                         FLASH_READMODE_DMACC(g_flash_instance.modeConfig.readSingleWord.readDmaccWord);
    FLASH->CMD = 3;
    while (!(FLASH->INT_STATUS & FLASH_INT_STATUS_DONE_MASK));
    result = FLASH->INT_STATUS;

    /* Report failure in case of errors */
    result_mask = FLASH_INT_STATUS_FAIL_MASK | FLASH_INT_STATUS_ERR_MASK | FLASH_INT_STATUS_ECC_ERR_MASK;
    if (result_mask & result)
        return -1;

    return 0;
}




