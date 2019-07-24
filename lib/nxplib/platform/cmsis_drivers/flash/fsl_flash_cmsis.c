/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 * Copyright (c) 2016, Freescale Semiconductor, Inc. Not a Contribution.
 * Copyright 2016-2017 NXP. Not a Contribution.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fsl_flash_cmsis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Alignment utility. */
#ifndef ALIGN_DOWN
#define ALIGN_DOWN(x, a) ((x) & (uint32_t)(-((int32_t)(a))))
#endif

/*!
 * @brief Redefines some flash features.
 * @{
 */
#if defined(FSL_FEATURE_FLASH_CURRENT_CORE_ID)
#if (FSL_FEATURE_FLASH_CURRENT_CORE_ID == 0u)
#define MAIN_FLASH_FEATURE_PFLASH_BLOCK_WRITE_UNIT_SIZE FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE
#define SECONDARY_FLASH_FEATURE_PFLASH_BLOCK_WRITE_UNIT_SIZE FSL_FEATURE_FLASH_PFLASH_1_BLOCK_WRITE_UNIT_SIZE
#elif (FSL_FEATURE_FLASH_CURRENT_CORE_ID == 1u)
#define MAIN_FLASH_FEATURE_PFLASH_BLOCK_WRITE_UNIT_SIZE FSL_FEATURE_FLASH_PFLASH_1_BLOCK_WRITE_UNIT_SIZE
#define SECONDARY_FLASH_FEATURE_PFLASH_BLOCK_WRITE_UNIT_SIZE FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE
#endif
#else
#define MAIN_FLASH_FEATURE_PFLASH_BLOCK_WRITE_UNIT_SIZE FSL_FEATURE_FLASH_PFLASH_BLOCK_WRITE_UNIT_SIZE
#endif
/*@}*/

/*! @brief CMSIS Flash development driver version*/
#define ARM_FLASH_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/*!
 * @brief CMSIS Flash info type
 */
typedef enum _flash_info_type
{
    kFlashInfoType_Pflash0 = 0x00u,
    kFlashInfoType_Dflash0 = 0x01u,
    kFlashInfoType_Pflash1 = 0x02u,
    kFlashInfoType_Dflash1 = 0x03u
} flash_info_type_t;

/*!
 * @brief CMSIS Flash resource
 */
typedef struct _cmsis_flash_resource
{
    uint32_t instance;
    flash_info_type_t infoType;
} cmsis_flash_resource_t;

/*!
 * @brief CMSIS Flash sector info
 */
typedef struct _cmsis_flash_sector
{
    uint32_t start;
    uint32_t end;
} cmsis_flash_sector_t;

/*!
 * @brief CMSIS Flash info
 */
typedef struct _cmsis_flash_info
{
    ARM_FLASH_SECTOR *sector_info;
    uint32_t sector_count;
    uint32_t sector_size;
    uint32_t page_size;
    uint32_t program_unit;
    uint8_t erased_value;
} cmsis_flash_info_t;

/*!
 * @brief CMSIS Flash driver state
 */
typedef struct _cmsis_flash_driver_state
{
    cmsis_flash_resource_t resource;
    ARM_FLASH_INFO *info;
    ARM_FLASH_STATUS status;
    ARM_Flash_SignalEvent_t cb_event;
#if FLASH_SSD_IS_SECONDARY_FLASH_ENABLED
    flash_config_t config[2];
#else
    flash_config_t config;
#endif
} cmsis_flash_driver_state_t;

/*!
 * @brief CMSIS Flash constants
 */
enum _flash_memory_constants
{
    kFlashMemory_ErasedValue = 0xFFu
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*! @brief Get flash resource according to the provided memory range.*/
static int32_t FLASH_GetResource(cmsis_flash_resource_t *resource, uint32_t address, uint32_t lengthInBytes);

/*! @brief Convert vendor flash driver status to standard ARM driver status.*/
static int32_t FLASH_ConvertErrorStatus(status_t status);

/*! @brief Update flash info for CMSIS driver.*/
static void FLASH_UpdateInfo(flash_info_type_t infoType);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief CMSIS Flash driver version*/
static const ARM_DRIVER_VERSION s_flashDriverVersion = {ARM_FLASH_API_VERSION, ARM_FLASH_DRV_VERSION};

/*! @brief CMSIS Flash driver Capabilities*/
static const ARM_FLASH_CAPABILITIES s_flashDriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    0  /* erase_chip */
};

/*! @brief CMSIS Flash sector */
static cmsis_flash_sector_t s_firstFlashSector;

/*! @brief CMSIS Flash info */
static cmsis_flash_info_t s_flashInfo;

/*! @brief CMSIS Flash driver state */
static cmsis_flash_driver_state_t s_flashDriverState;

/*******************************************************************************
 * Code
 ******************************************************************************/

static int32_t FLASH_GetResource(cmsis_flash_resource_t *resource, uint32_t address, uint32_t lengthInBytes)
{
    uint32_t flashIndex = 0;
#if !FLASH_SSD_IS_SECONDARY_FLASH_ENABLED
    uint32_t maxFlashInstance = 1;
#else
    uint32_t maxFlashInstance = 2;
    for (; flashIndex < maxFlashInstance; flashIndex++)
#endif
    {
        uint32_t DFlashBlockBase = s_flashDriverState.config[flashIndex].DFlashBlockBase;
        uint32_t DFlashTotalSize = s_flashDriverState.config[flashIndex].DFlashTotalSize;
        uint32_t PFlashBlockBase = s_flashDriverState.config[flashIndex].PFlashBlockBase;
        uint32_t PFlashTotalSize = s_flashDriverState.config[flashIndex].PFlashTotalSize;

        if ((address >= DFlashBlockBase) && ((address + lengthInBytes) <= (DFlashBlockBase + DFlashTotalSize)))
        {
            resource->instance = flashIndex;
            if (resource->instance == 0)
            {
                resource->infoType = kFlashInfoType_Dflash0;
            }
            else
            {
                resource->infoType = kFlashInfoType_Dflash1;
            }
            return ARM_DRIVER_OK;
        }

        if ((address >= PFlashBlockBase) && ((address + lengthInBytes) <= (PFlashBlockBase + PFlashTotalSize)))
        {
            resource->instance = flashIndex;
            if (resource->instance == 0)
            {
                resource->infoType = kFlashInfoType_Pflash0;
            }
            else
            {
                resource->infoType = kFlashInfoType_Pflash1;
            }
            return ARM_DRIVER_OK;
        }
    }

    return ARM_DRIVER_ERROR_PARAMETER;
}

static int32_t FLASH_ConvertErrorStatus(status_t status)
{
    switch (status)
    {
        case kStatus_FLASH_InvalidArgument:
        case kStatus_FLASH_AlignmentError:
        case kStatus_FLASH_AddressError:
        case kStatus_FLASH_ExecuteInRamFunctionNotReady:
            return ARM_DRIVER_ERROR_PARAMETER;
        case kStatus_FLASH_AccessError:
        case kStatus_FLASH_ProtectionViolation:
        case kStatus_FLASH_CommandFailure:
            s_flashDriverState.status.error = 1;
            return ARM_DRIVER_ERROR;
        case kStatus_FLASH_Success:
        default:
            s_flashDriverState.status.error = 0;
            return ARM_DRIVER_OK;
    }
}

static void FLASH_UpdateInfo(flash_info_type_t infoType)
{
    switch (infoType)
    {
#if FLASH_SSD_IS_SECONDARY_FLASH_ENABLED
        case kFlashInfoType_Pflash1:
            s_flashInfo.sector_count =
                s_flashDriverState.config[1].PFlashTotalSize / s_flashDriverState.config[1].PFlashSectorSize;
            s_flashInfo.sector_size  = s_flashDriverState.config[1].PFlashSectorSize;
            s_flashInfo.program_unit = SECONDARY_FLASH_FEATURE_PFLASH_BLOCK_WRITE_UNIT_SIZE;
            s_firstFlashSector.start = s_flashDriverState.config[1].PFlashBlockBase;
            break;
#endif

#if FLASH_SSD_IS_FLEXNVM_ENABLED
        case kFlashInfoType_Dflash0:
            s_flashInfo.sector_count =
                s_flashDriverState.config[0].DFlashTotalSize / FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SECTOR_SIZE;
            s_flashInfo.sector_size  = FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_SECTOR_SIZE;
            s_flashInfo.program_unit = FSL_FEATURE_FLASH_FLEX_NVM_BLOCK_WRITE_UNIT_SIZE;
            s_firstFlashSector.start = s_flashDriverState.config[0].DFlashBlockBase;

            break;
#endif

        case kFlashInfoType_Pflash0:
        default:
            s_flashInfo.sector_count =
                s_flashDriverState.config[0].PFlashTotalSize / s_flashDriverState.config[0].PFlashSectorSize;
            s_flashInfo.sector_size  = s_flashDriverState.config[0].PFlashSectorSize;
            s_flashInfo.program_unit = MAIN_FLASH_FEATURE_PFLASH_BLOCK_WRITE_UNIT_SIZE;
            s_firstFlashSector.start = s_flashDriverState.config[0].PFlashBlockBase;
            break;
    }

    s_firstFlashSector.end = s_firstFlashSector.start + s_flashInfo.sector_size - 1;

    /* Page program is not supported by Vendor FLASH driver */
    s_flashInfo.page_size    = s_flashInfo.program_unit;
    s_flashInfo.erased_value = kFlashMemory_ErasedValue;
}

static ARM_DRIVER_VERSION FLASH_GetVersion(void)
{
    return s_flashDriverVersion;
}

static int32_t FLASH_Uninitialize(void)
{
    /* Cleanup CMSIS Flash sector Structure */
    memset(&s_firstFlashSector, 0, sizeof(cmsis_flash_sector_t));
    /* Cleanup CMSIS Flash info Structure */
    memset(&s_flashInfo, 0, sizeof(cmsis_flash_info_t));
    /* Cleanup CMSIS Flash driver Structure */
    memset(&s_flashDriverState, 0, sizeof(cmsis_flash_driver_state_t));

    return ARM_DRIVER_OK;
}

static ARM_FLASH_CAPABILITIES FLASH_GetCapabilities(void)
{
    return s_flashDriverCapabilities;
}

static int32_t FLASH_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    uint32_t flashIndex = 0;
#if !FLASH_SSD_IS_SECONDARY_FLASH_ENABLED
    uint32_t maxFlashInstance = 1;
#else
    uint32_t maxFlashInstance = 2;
#endif

    /* Uninitialize driver to cleanup all variables */
    FLASH_Uninitialize();

    s_flashInfo.sector_info = (ARM_FLASH_SECTOR *)&s_firstFlashSector;
    s_flashDriverState.info = (ARM_FLASH_INFO *)&s_flashInfo;

    /* Set default flash status */
    s_flashDriverState.status.busy  = 0;
    s_flashDriverState.status.error = 0;

    /* Initialize flash driver*/
    for (; flashIndex < maxFlashInstance; flashIndex++)
    {
        /* Note: For dual core device (such as K3S), we must set correct FlashMemoryIndex*/
        FLASH_SetProperty(&s_flashDriverState.config[flashIndex], kFLASH_PropertyFlashMemoryIndex, flashIndex);
        status_t status = FLASH_Init(&s_flashDriverState.config[flashIndex]);
        if (status != kStatus_FLASH_Success)
        {
            return FLASH_ConvertErrorStatus(status);
        }
    }

    /* Update flash info to PFlash0 by default */
    s_flashDriverState.resource.instance = 0;
    s_flashDriverState.resource.infoType = kFlashInfoType_Pflash0;
    FLASH_UpdateInfo(s_flashDriverState.resource.infoType);

    /* Set callback function */
    s_flashDriverState.cb_event = cb_event;

    return ARM_DRIVER_OK;
}

int32_t FLASH_PowerControl(ARM_POWER_STATE state)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            break;

        case ARM_POWER_LOW:
            break;

        case ARM_POWER_FULL:
            break;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t FLASH_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    /* Check range to find correct flash resource */
    int32_t result = FLASH_GetResource(&s_flashDriverState.resource, addr, cnt);
    if (result == ARM_DRIVER_OK)
    {
        /* Update flash info */
        FLASH_UpdateInfo(s_flashDriverState.resource.infoType);

        memcpy((uint8_t *)addr, data, cnt);
    }

    return result;
}

static int32_t FLASH_ProgramData(uint32_t addr, const void *data, uint32_t cnt)
{
    /* Check range to find correct flash resource */
    int32_t result = FLASH_GetResource(&s_flashDriverState.resource, addr, cnt);
    if (result == ARM_DRIVER_OK)
    {
        /* Update flash info */
        FLASH_UpdateInfo(s_flashDriverState.resource.infoType);
        /* Issue flash program API */
        status_t status = FLASH_Program(&s_flashDriverState.config[s_flashDriverState.resource.instance], addr,
                                        (uint32_t *)data, cnt);
        return FLASH_ConvertErrorStatus(status);
    }

    return result;
}

static int32_t FLASH_EraseSector(uint32_t addr)
{
    int32_t result;
    uint32_t alignedAddress;

    /* Check range to find correct flash resource */
    result = FLASH_GetResource(&s_flashDriverState.resource, addr, 1);
    if (result != ARM_DRIVER_OK)
    {
        return result;
    }

    /* Update flash info */
    FLASH_UpdateInfo(s_flashDriverState.resource.infoType);

    /* Get corresponding sector size and align start address for FLASH_Erase API */
    alignedAddress = ALIGN_DOWN(addr, s_flashDriverState.info->sector_size);

    if (result == ARM_DRIVER_OK)
    {
        /* Issue flash erase API */
        status_t status = FLASH_Erase(&s_flashDriverState.config[s_flashDriverState.resource.instance], alignedAddress,
                                      s_flashDriverState.info->sector_size, kFLASH_ApiEraseKey);
        return FLASH_ConvertErrorStatus(status);
    }

    return result;
}

static int32_t FLASH_EraseChip(void)
{
    /* Issue flash erase all API */
    status_t status = FLASH_EraseAll(&s_flashDriverState.config[0], kFLASH_ApiEraseKey);
    return FLASH_ConvertErrorStatus(status);
}

static ARM_FLASH_STATUS FLASH_GetStatus(void)
{
    return s_flashDriverState.status;
}

static ARM_FLASH_INFO *FLASH_GetInfo(void)
{
    /* It always return the flash info last time we operated*/
    return s_flashDriverState.info;
}

ARM_DRIVER_FLASH Driver_FLASH = {FLASH_GetVersion,   FLASH_GetCapabilities, FLASH_Initialize,  FLASH_Uninitialize,
                                 FLASH_PowerControl, FLASH_ReadData,        FLASH_ProgramData, FLASH_EraseSector,
                                 FLASH_EraseChip,    FLASH_GetStatus,       FLASH_GetInfo};
