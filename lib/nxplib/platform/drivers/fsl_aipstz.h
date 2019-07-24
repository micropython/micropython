/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_AIPSTZ_H_
#define _FSL_AIPSTZ_H_

#include "fsl_common.h"

/*!
 * @addtogroup aipstz
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_AIPSTZ_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0 */
/*@}*/

/*! @brief List of AIPSTZ privilege configuration.*/
typedef enum _aipstz_master_privilege_level
{
    kAIPSTZ_MasterBufferedWriteEnable   = (1U << 3), /*!< Write accesses from this master are allowed to be buffered. */
    kAIPSTZ_MasterTrustedForReadEnable  = (1U << 2), /*!< This master is trusted for read accesses. */
    kAIPSTZ_MasterTrustedForWriteEnable = (1U << 1), /*!< This master is trusted for write accesses. */
    kAIPSTZ_MasterForceUserModeEnable   = 1U         /*!< Accesses from this master are forced to user-mode. */
} aipstz_master_privilege_level_t;

/*! @brief List of AIPSTZ masters. Organized by width for the 8-15 bits and shift for lower 8 bits.*/
typedef enum _aipstz_master
{
    kAIPSTZ_Master0 = (0x400U | 28U),
    kAIPSTZ_Master1 = (0x400U | 24U),
    kAIPSTZ_Master2 = (0x400U | 20U),
    kAIPSTZ_Master3 = (0x400U | 16U),
    kAIPSTZ_Master5 = (0x400U | 8U)
} aipstz_master_t;

/*! @brief List of AIPSTZ peripheral access control configuration.*/
typedef enum _aipstz_peripheral_access_control
{
    kAIPSTZ_PeripheralAllowUntrustedMaster = 1U,
    kAIPSTZ_PeripheralWriteProtected       = (1U << 1),
    kAIPSTZ_PeripheralRequireSupervisor    = (1U << 2),
    kAIPSTZ_PeripheralAllowBufferedWrite   = (1U << 3)
} aipstz_peripheral_access_control_t;

/*! @brief List of AIPSTZ peripherals. Organized by register offset for higher 32 bits, width for the 8-15 bits and
 * shift for lower 8 bits.*/
typedef enum _aipstz_peripheral
{
    kAIPSTZ_Peripheral0  = ((0x40 << 16) | (4 << 8) | 28),
    kAIPSTZ_Peripheral1  = ((0x40 << 16) | (4 << 8) | 24),
    kAIPSTZ_Peripheral2  = ((0x40 << 16) | (4 << 8) | 20),
    kAIPSTZ_Peripheral3  = ((0x40 << 16) | (4 << 8) | 16),
    kAIPSTZ_Peripheral4  = ((0x40 << 16) | (4 << 8) | 12),
    kAIPSTZ_Peripheral5  = ((0x40 << 16) | (4 << 8) | 8),
    kAIPSTZ_Peripheral6  = ((0x40 << 16) | (4 << 8) | 4),
    kAIPSTZ_Peripheral7  = ((0x40 << 16) | (4 << 8) | 0),
    kAIPSTZ_Peripheral8  = ((0x44 << 16) | (4 << 8) | 28),
    kAIPSTZ_Peripheral9  = ((0x44 << 16) | (4 << 8) | 24),
    kAIPSTZ_Peripheral10 = ((0x44 << 16) | (4 << 8) | 20),
    kAIPSTZ_Peripheral11 = ((0x44 << 16) | (4 << 8) | 16),
    kAIPSTZ_Peripheral12 = ((0x44 << 16) | (4 << 8) | 12),
    kAIPSTZ_Peripheral13 = ((0x44 << 16) | (4 << 8) | 8),
    kAIPSTZ_Peripheral14 = ((0x44 << 16) | (4 << 8) | 4),
    kAIPSTZ_Peripheral15 = ((0x44 << 16) | (4 << 8) | 0),
    kAIPSTZ_Peripheral16 = ((0x48 << 16) | (4 << 8) | 28),
    kAIPSTZ_Peripheral17 = ((0x48 << 16) | (4 << 8) | 24),
    kAIPSTZ_Peripheral18 = ((0x48 << 16) | (4 << 8) | 20),
    kAIPSTZ_Peripheral19 = ((0x48 << 16) | (4 << 8) | 16),
    kAIPSTZ_Peripheral20 = ((0x48 << 16) | (4 << 8) | 12),
    kAIPSTZ_Peripheral21 = ((0x48 << 16) | (4 << 8) | 8),
    kAIPSTZ_Peripheral22 = ((0x48 << 16) | (4 << 8) | 4),
    kAIPSTZ_Peripheral23 = ((0x48 << 16) | (4 << 8) | 0),
    kAIPSTZ_Peripheral24 = ((0x4C << 16) | (4 << 8) | 28),
    kAIPSTZ_Peripheral25 = ((0x4C << 16) | (4 << 8) | 24),
    kAIPSTZ_Peripheral26 = ((0x4C << 16) | (4 << 8) | 20),
    kAIPSTZ_Peripheral27 = ((0x4C << 16) | (4 << 8) | 16),
    kAIPSTZ_Peripheral28 = ((0x4C << 16) | (4 << 8) | 12),
    kAIPSTZ_Peripheral29 = ((0x4C << 16) | (4 << 8) | 8),
    kAIPSTZ_Peripheral30 = ((0x4C << 16) | (4 << 8) | 4),
    kAIPSTZ_Peripheral31 = ((0x4C << 16) | (4 << 8) | 0),
    kAIPSTZ_Peripheral32 = ((0x50 << 16) | (4 << 8) | 28),
    kAIPSTZ_Peripheral33 = ((0x50 << 16) | (4 << 8) | 24)
} aipstz_peripheral_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Configure the privilege level for master.
 *
 * @param base    AIPSTZ peripheral base pointer
 * @param master  Masters for AIPSTZ.
 * @param privilegeConfig Configuration is ORed from @aipstz_master_privilege_level_t.
 */
void AIPSTZ_SetMasterPriviledgeLevel(AIPSTZ_Type *base, aipstz_master_t master, uint32_t privilegeConfig);

/*!
 * @brief Configure the access for peripheral.
 *
 * @param base    AIPSTZ peripheral base pointer
 * @param master  Peripheral for AIPSTZ.
 * @param accessControl Configuration is ORed from @aipstz_peripheral_access_control_t.
 */
void AIPSTZ_SetPeripheralAccessControl(AIPSTZ_Type *base, aipstz_peripheral_t peripheral, uint32_t accessControl);

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_AIPSTZ_H_ */
