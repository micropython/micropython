/*
 * This file is part of the Micro Python project, http://micropython.org/
 */

/**
  ******************************************************************************
  * @file    usbd_storage_msd.c
  * @author  MCD application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the disk operations functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  * Heavily modified by dpgeorge for Micro Python.
  *
  ******************************************************************************
  */

#include <stdint.h>

#include "usbd_cdc_msc_hid.h"
#include "usbd_msc_storage.h"

#include "py/misc.h"
#include "storage.h"
#include "sdcard.h"

// These are needed to support removal of the medium, so that the USB drive
// can be unmounted, and won't be remounted automatically.
static uint8_t flash_started = 0;

#if MICROPY_HW_HAS_SDCARD
static uint8_t sdcard_started = 0;
#endif

/******************************************************************************/
// Callback functions for when the internal flash is the mass storage device

static const int8_t FLASH_STORAGE_Inquirydata[] = { // 36 bytes
    // LUN 0
    0x00,
    0x80, // 0x00 for a fixed drive, 0x80 for a removable drive
    0x02,
    0x02,
    (STANDARD_INQUIRY_DATA_LEN - 5),
    0x00,
    0x00,
    0x00,
    'u', 'P', 'y', ' ', ' ', ' ', ' ', ' ', // Manufacturer : 8 bytes
    'm', 'i', 'c', 'r', 'o', 'S', 'D', ' ', // Product      : 16 Bytes
    'F', 'l', 'a', 's', 'h', ' ', ' ', ' ',
    '1', '.', '0' ,'0',                     // Version      : 4 Bytes
};

/**
  * @brief  Initialize the storage medium
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t FLASH_STORAGE_Init(uint8_t lun) {
    storage_init();
    flash_started = 1;
    return 0;
}

/**
  * @brief  return medium capacity and block size
  * @param  lun : logical unit number
  * @param  block_num :  number of physical block
  * @param  block_size : size of a physical block
  * @retval Status
  */
int8_t FLASH_STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size) {
    *block_size = storage_get_block_size();
    *block_num = storage_get_block_count();
    return 0;
}

/**
  * @brief  check whether the medium is ready
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t FLASH_STORAGE_IsReady(uint8_t lun) {
    if (flash_started) {
        return 0;
    }
    return -1;
}

/**
  * @brief  check whether the medium is write-protected
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t FLASH_STORAGE_IsWriteProtected(uint8_t lun) {
    return  0;
}

// Remove the lun
int8_t FLASH_STORAGE_StartStopUnit(uint8_t lun, uint8_t started) {
    flash_started = started;
    return 0;
}

int8_t FLASH_STORAGE_PreventAllowMediumRemoval(uint8_t lun, uint8_t param) {
    // sync the flash so that the cache is cleared and the device can be unplugged/turned off
    storage_flush();
    return 0;
}

/**
  * @brief  Read data from the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to save data
  * @param  blk_addr :  address of 1st block to be read
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t FLASH_STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
    storage_read_blocks(buf, blk_addr, blk_len);
    /*
    for (int i = 0; i < blk_len; i++) {
        if (!storage_read_block(buf + i * FLASH_BLOCK_SIZE, blk_addr + i)) {
            return -1;
        }
    }
    */
    return 0;
}

/**
  * @brief  Write data to the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to write from
  * @param  blk_addr :  address of 1st block to be written
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t FLASH_STORAGE_Write (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
    storage_write_blocks(buf, blk_addr, blk_len);
    /*
    for (int i = 0; i < blk_len; i++) {
        if (!storage_write_block(buf + i * FLASH_BLOCK_SIZE, blk_addr + i)) {
            return -1;
        }
    }
    */
    return 0;
}

/**
  * @brief  Return number of supported logical unit
  * @param  None
  * @retval number of logical unit
  */
int8_t FLASH_STORAGE_GetMaxLun(void) {
    return 0;
}

const USBD_StorageTypeDef USBD_FLASH_STORAGE_fops = {
    FLASH_STORAGE_Init,
    FLASH_STORAGE_GetCapacity,
    FLASH_STORAGE_IsReady,
    FLASH_STORAGE_IsWriteProtected,
    FLASH_STORAGE_StartStopUnit,
    FLASH_STORAGE_PreventAllowMediumRemoval,
    FLASH_STORAGE_Read,
    FLASH_STORAGE_Write,
    FLASH_STORAGE_GetMaxLun,
    (int8_t *)FLASH_STORAGE_Inquirydata,
};

/******************************************************************************/
// Callback functions for when the SD card is the mass storage device

#if MICROPY_HW_HAS_SDCARD

static const int8_t SDCARD_STORAGE_Inquirydata[] = { // 36 bytes
    // LUN 0
    0x00,
    0x80, // 0x00 for a fixed drive, 0x80 for a removable drive
    0x02,
    0x02,
    (STANDARD_INQUIRY_DATA_LEN - 5),
    0x00,
    0x00,
    0x00,
    'u', 'P', 'y', ' ', ' ', ' ', ' ', ' ', // Manufacturer : 8 bytes
    'm', 'i', 'c', 'r', 'o', 'S', 'D', ' ', // Product      : 16 Bytes
    'S', 'D', ' ', 'c', 'a', 'r', 'd', ' ',
    '1', '.', '0' ,'0',                     // Version      : 4 Bytes
};

/**
  * @brief  Initialize the storage medium
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t SDCARD_STORAGE_Init(uint8_t lun) {
    /*
#ifndef USE_STM3210C_EVAL 
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  if( SD_Init() != 0)
  {
    return (-1);
  }
  */
    if (!sdcard_power_on()) {
        return -1;
    }
    sdcard_started = 1;
    return 0;

}

/**
  * @brief  return medium capacity and block size
  * @param  lun : logical unit number
  * @param  block_num :  number of physical block
  * @param  block_size : size of a physical block
  * @retval Status
  */
int8_t SDCARD_STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size) {
/*
#ifdef USE_STM3210C_EVAL   
  SD_CardInfo SDCardInfo;
  SD_GetCardInfo(&SDCardInfo);  
#else
  if(SD_GetStatus() != 0 ) {
    return (-1); 
  }   
#endif  
  */

    *block_size = SDCARD_BLOCK_SIZE;
    *block_num =  sdcard_get_capacity_in_bytes() / SDCARD_BLOCK_SIZE;

    return 0;
}

/**
  * @brief  check whether the medium is ready
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t SDCARD_STORAGE_IsReady(uint8_t lun) {
    if (sdcard_started) {
        return 0;
    }
    return -1;
}

/**
  * @brief  check whether the medium is write-protected
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t SDCARD_STORAGE_IsWriteProtected(uint8_t lun) {
    return 0;
}

// Remove the lun
int8_t SDCARD_STORAGE_StartStopUnit(uint8_t lun, uint8_t started) {
    sdcard_started = started;
    return 0;
}

int8_t SDCARD_STORAGE_PreventAllowMediumRemoval(uint8_t lun, uint8_t param) {
    return 0;
}

/**
  * @brief  Read data from the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to save data
  * @param  blk_addr :  address of 1st block to be read
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t SDCARD_STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
    if (sdcard_read_blocks(buf, blk_addr, blk_len) != 0) {
        return -1;
    }
    return 0;
}

/**
  * @brief  Write data to the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to write from
  * @param  blk_addr :  address of 1st block to be written
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t SDCARD_STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len) {
    if (sdcard_write_blocks(buf, blk_addr, blk_len) != 0) {
        return -1;
    }
    return 0;
}

/**
  * @brief  Return number of supported logical unit
  * @param  None
  * @retval number of logical unit
  */
int8_t SDCARD_STORAGE_GetMaxLun(void) {
    return 0;
}

const USBD_StorageTypeDef USBD_SDCARD_STORAGE_fops = {
    SDCARD_STORAGE_Init,
    SDCARD_STORAGE_GetCapacity,
    SDCARD_STORAGE_IsReady,
    SDCARD_STORAGE_IsWriteProtected,
    SDCARD_STORAGE_StartStopUnit,
    SDCARD_STORAGE_PreventAllowMediumRemoval,
    SDCARD_STORAGE_Read,
    SDCARD_STORAGE_Write,
    SDCARD_STORAGE_GetMaxLun,
    (int8_t *)SDCARD_STORAGE_Inquirydata,
};

#endif // MICROPY_HW_HAS_SDCARD
