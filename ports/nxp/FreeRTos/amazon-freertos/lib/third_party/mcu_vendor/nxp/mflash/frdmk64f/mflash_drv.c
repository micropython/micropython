/*
 * Copyright 2017 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flash.h"
#include "mflash_drv.h"
#include "pin_mux.h"
#include <stdbool.h>

/* Temporary sector storage. Use uint32_t type to force 4B alignment and
 * improve copy operation */
static uint32_t g_flashm_sector[MFLASH_SECTOR_SIZE / sizeof(uint32_t)];

/*! @brief Flash driver Structure */
static flash_config_t s_flashDriver;

/* API - initialize 'mflash' */
int32_t mflash_drv_init(void)
{
    int32_t result = 0;
    /* Clean up Flash driver Structure*/
    memset(&s_flashDriver, 0, sizeof(flash_config_t));
    /* Setup flash driver structure for device and initialize variables. */
    result = FLASH_Init(&s_flashDriver);
    return result;
}

/* Internal - erase single sector */
static int32_t mflash_drv_sector_erase(uint32_t sector_addr)
{
    int32_t result = 0;
    result = FLASH_Erase(&s_flashDriver, sector_addr, MFLASH_SECTOR_SIZE, kFTFx_ApiEraseKey);
    return result;
}

/* Internal - write single sector */
static int32_t mflash_drv_sector_program(uint32_t sector_addr, uint32_t *sector_data, uint32_t sector_data_length)
{
    int32_t result = 0;
    if (false == mflash_drv_is_sector_aligned((uint32_t)sector_addr))
        return -1;

    result = FLASH_Program(&s_flashDriver, (uint32_t)sector_addr, (uint8_t*)sector_data, sector_data_length);
    return result;
}

/* Internal - write data of 'data_len' to single sector 'sector_addr', starting from 'sect_off' */
int32_t mflash_drv_sector_write(uint32_t sector_addr, uint32_t sect_off, uint8_t *data, uint32_t data_len)
{
    /* Address not aligned to sector boundary */
    if (false == mflash_drv_is_sector_aligned((uint32_t)sector_addr))
        return -1;
    /* Offset + length exceeed sector size */
    if (sect_off + data_len > MFLASH_SECTOR_SIZE)
        return -1;

    /* Copy old sector data by 4B in each loop to buffer */
    for (uint32_t i = 0; i < sizeof(g_flashm_sector) / sizeof(g_flashm_sector[0]); i++)
    {
        g_flashm_sector[i] = *((uint32_t *)(sector_addr) + i);
    }

    /* Copy custom data ( 1B in each loop ) to buffer at specific position */
    for (uint32_t i = 0; i < data_len; i++)
    {
        ((uint8_t *)g_flashm_sector)[sect_off + i] = data[i];
    }

    /* Erase flash */
    if (0 != mflash_drv_sector_erase(sector_addr))
        return -2;

    /* Program data */
    if (0 != mflash_drv_sector_program(sector_addr, g_flashm_sector, sizeof(g_flashm_sector)))
        return -2;

    return 0;
}

/* Write data to flash, cannot be invoked directly, requires calling wrapper in non XIP memory */
int32_t mflash_drv_write_internal(void *any_addr, uint8_t *data, uint32_t data_len)
{
    uint32_t sect_size = MFLASH_SECTOR_SIZE;
    /* Interval <0, sector_size) */
    uint32_t to_write = 0;
    /* Interval (data_len, 0>  */
    uint32_t to_remain = data_len;
    int32_t result = 0;

    for (
        /* Calculate address of first sector */
        uint32_t sect_a = mflash_drv_addr_to_sector_addr((uint32_t)any_addr),
                 /* and first sector offset */
        sect_of = mflash_drv_addr_to_sector_of((uint32_t)any_addr),
                 /* and set first data offset to 0*/
        data_of = 0;
        /* Continue until sector address exceed target adddress + data_length */
        sect_a < ((uint32_t)any_addr) + data_len;
        /* Move to next sector */
        sect_a += sect_size,
                 /* and move pointer to data */
        data_of += to_write)
    {
        /* If remaining data is exceed 'sector_size', write 'sector_size' length */
        if (to_remain > sect_size - sect_of)
        {
            to_write = sect_size - sect_of;
            to_remain = to_remain - to_write;
        }
        /* else write remaining data length */
        else
        {
            to_write = to_remain;
            to_remain = 0;
        }

        /* Write at 'sect_a' sector, starting at 'sect_of' using '&data[data_of]' of length 'to_write' */
        result = mflash_drv_sector_write(sect_a, sect_of, &data[data_of], to_write);
        if (0 != result)
            return -1;
        /* Only first sector is allowed to have an offset */
        sect_of = 0;
    }

    return 0;
}

/* Calling wrapper for 'mflash_drv_write_internal'.
 * Write 'data' of 'data_len' to 'any_addr' - which doesn't have to be sector aligned.
 * NOTE: Don't try to store constant data that are located in XIP !!
 */
int32_t mflash_drv_write(void *any_addr, uint8_t *data, uint32_t data_len)
{
    volatile int32_t result;
    result = mflash_drv_write_internal(any_addr, data, data_len);
    return result;
}
