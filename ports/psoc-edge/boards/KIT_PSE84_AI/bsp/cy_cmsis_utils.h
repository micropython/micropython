/***************************************************************************//**
* \file cy_cmsis_utils.h
* \version 1.00
*
* \brief
* Utility wrapper around CMSIS functionality
*
********************************************************************************
* \copyright
* Copyright(c) 2025 Infineon Technologies AG or an affiliate of
* Infineon Technologies AG
*
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/


#if !defined (CY_CMSIS_UTILS_H)
#define CY_CMSIS_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "cy_device.h"
#include "cmsis_compiler.h"
#include "armv8m_mpu.h"


/** \addtogroup group_cmsis_utils
  * \{
  *
  * This module provides wrapper functions and configuration structs to facilitate the initialization of
  * the following standard ARM hardware blocks:
  * * SAU
  * * MPU
  *
  * \defgroup group_cmsis_functions Functions
  * \defgroup group_cmsis_data_structures Data Structures
  */

/** \addtogroup group_cmsis_data_structures
  * \{
  */

#if defined(CY_PDL_TZ_ENABLED) || defined(DOXYGEN)
/** Configuration for a SAU entry */
typedef struct {
    uint8_t     reg_num;   /**< Region number. */
    uint32_t    base_addr; /**< Base address of SAU region. */
    uint32_t    size;      /**< Size of SAU region. */
    bool        nsc;       /**< Is this region Non-Secure Callable? */
} cy_stc_sau_config_t;
#endif /* defined(CY_PDL_TZ_ENABLED) || defined(DOXYGEN) */

/** Configuration for an MPU region */
typedef struct {
    uint8_t     reg_num;            /**< Region number. */
    uint32_t    base_addr;          /**< Base address of region. */
    uint32_t    end_addr;           /**< End address of region. */
    bool        writable;           /**< Is the memory region writeable */
    bool        require_privileged; /**< Is access to this memory restricted to privileged mode */
    bool        shareable;          /**< Is this memory shared with other bus masters */
    bool        executable;         /**< Is execution permitted from this memory? */
    uint8_t     cacheable;          /**< On CPUs with a cache, should access to this memory be cached? */
    bool        is_device;          /**< Is this memory used for peripheral control registers */
    uint8_t     device_attrs;       /**< Only applicable if is_device is true. Attributes for device memory access */
} cy_stc_mpu_config_t;

/** \} group_cmsis_data_structures */

/** \addtogroup group_cmsis_functions
  * \{
  */

#if defined(CY_PDL_TZ_ENABLED) || defined(DOXYGEN)
/** Applies the specified configuration to the SAU
  *
  * \param base Base address for the SAU
  * \param region_config Array of configurations structs for each SAU region
  * \param region_count Length of the region_config array
  */
static inline void Cy_SAU_Init(SAU_Type* base, const cy_stc_sau_config_t* region_config, uint8_t region_count)
{
    /** Check that we have not overflowed the SAU region limit */
    CY_ASSERT(region_count <= (base->TYPE & SAU_TYPE_SREGION_Msk));
    base->CTRL |= SAU_CTRL_ENABLE_Msk;
    for (uint8_t i = 0U ; i < region_count ; ++i)
    {
        base->RNR = region_config[i].reg_num;
        base->RBAR = (region_config[i].base_addr & SAU_RBAR_BADDR_Msk);
        base->RLAR = (((region_config[i].base_addr + region_config[i].size - 1U) & SAU_RLAR_LADDR_Msk) | \
            (region_config[i].nsc ? SAU_RLAR_NSC_Msk : 0U) | SAU_RLAR_ENABLE_Msk);
    }
}
#endif /* defined(CY_PDL_TZ_ENABLED) || defined(DOXYGEN) */

/** Applies the specified configuration to the MPU for the current processor
  *
  * \param region_config Array of configurations structs for each MPU region
  * \param region_count Length of the region_config array
  */
static inline void Cy_MPU_Init(const cy_stc_mpu_config_t* region_config, uint8_t region_count)
{
    __DMB();

    ARM_MPU_Disable();

    for(uint8_t i = 0; i < region_count; ++i)
    {
        /* Bit 0 of AP indicates whether unprivileged access is allowed. Bit 1 indicates whether the memory is read-only */
        uint8_t access_permissions = 0u;
        if (false == region_config[i].require_privileged)
        {
            access_permissions |= 1u << 0;
        }
        if (false == region_config[i].writable)
        {
            access_permissions |= 1u << 1;
        }
        uint32_t rbarValue = ((region_config[i].base_addr & MPU_RBAR_BASE_Msk)                                // Set the base address'
                           | ((region_config[i].shareable << MPU_RBAR_SH_Pos) & MPU_RBAR_SH_Msk)              // Set Shareability (SH)
                           | ((access_permissions << MPU_RBAR_AP_Pos) & MPU_RBAR_AP_Msk)                      // Set Access Permissions (AP)
                           | ((region_config[i].executable ? 0 : 1 << MPU_RBAR_XN_Pos) & MPU_RBAR_XN_Msk));   // Set Execute Never (XN) bit
        uint32_t rlarValue = ((region_config[i].end_addr & MPU_RLAR_LIMIT_Msk)                                // Set the limit address
                           | ((region_config[i].reg_num  << MPU_RLAR_AttrIndx_Pos) & MPU_RLAR_AttrIndx_Msk)    // Attribute index
                           | (MPU_RLAR_EN_Msk));                                                              // Enable bit
        ARM_MPU_SetRegion(region_config[i].reg_num, rbarValue, rlarValue);
        if(region_config[i].is_device)
        {
            /* In the case of device memory, only the second "inner" attributes are relevant */
            ARM_MPU_SetMemAttr(region_config[i].reg_num, ARM_MPU_ATTR(0U, region_config[i].device_attrs));
        }
        else
        {
          /* The two arguments to ARM_MPU_ATTR are for outer and inner memory configuration. We want to configure them the same */
          ARM_MPU_SetMemAttr(region_config[i].reg_num, ARM_MPU_ATTR(region_config[i].cacheable, region_config[i].cacheable));
        }
    }

    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk);

    __DMB();
    __ISB();
}
/** \} group_cmsis_functions */

/** \} group_cmsis */

/** \cond INTERNAL
  * These defines form a private interface with the SAU personality
  */
/*******************************************************************************
*               NSC region addresses
*******************************************************************************/
#ifdef __ICCARM__
#pragma section = "NSC"
#ifndef NSC_START_ADDRESS
#define NSC_START_ADDRESS  (uint32_t) (__section_begin("NSC"))
#endif /* NSC_START_ADDRESS */
#ifndef NSC_SIZE
#define NSC_SIZE  (uint32_t) (__section_size("NSC"))
#endif /* NSC_SIZE */

#elif defined(__ARMCC_VERSION)

extern uint32_t Image$$app_codeNsc$$Base;
extern uint32_t Image$$app_codeNsc$$Length;
#ifndef NSC_START_ADDRESS
#define NSC_START_ADDRESS  (uint32_t)&Image$$app_codeNsc$$Base
#endif /* NSC_START_ADDRESS */
#ifndef NSC_SIZE
#define NSC_SIZE  (uint32_t)&Image$$app_codeNsc$$Length
#endif /* NSC_SIZE */

#elif defined(__llvm__) || defined (__GNUC__)

extern uint32_t __nsc_region_start;
extern uint32_t __nsc_region_size;
#ifndef NSC_START_ADDRESS
#define NSC_START_ADDRESS  (uint32_t)&__nsc_region_start
#endif /* NSC_START_ADDRESS */
#ifndef NSC_SIZE
#define NSC_SIZE  (uint32_t)&__nsc_region_size
#endif /* NSC_SIZE */

#else
#error "Unsupported toolchain"
#endif

/** \endcond */

#endif /* !defined (CY_CMSIS_UTILS_H) */
