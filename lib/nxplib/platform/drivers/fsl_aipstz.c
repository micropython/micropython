/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_aipstz.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.aipstz"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * brief Configure the privilege level for master.
 *
 * param base    AIPSTZ peripheral base pointer
 * param master  Masters for AIPSTZ.
 * param privilegeConfig Configuration is ORed from aipstz_master_privilege_level_t.
 */
void AIPSTZ_SetMasterPriviledgeLevel(AIPSTZ_Type *base, aipstz_master_t master, uint32_t privilegeConfig)
{
    uint32_t mask  = ((uint32_t)master >> 8) - 1;
    uint32_t shift = (uint32_t)master & 0xFF;
    base->MPR      = (base->MPR & (~(mask << shift))) | (privilegeConfig << shift);
}

/*!
 * brief Configure the access for peripheral.
 *
 * param base    AIPSTZ peripheral base pointer
 * param master  Peripheral for AIPSTZ.
 * param accessControl Configuration is ORed from aipstz_peripheral_access_control_t.
 */
void AIPSTZ_SetPeripheralAccessControl(AIPSTZ_Type *base, aipstz_peripheral_t peripheral, uint32_t accessControl)
{
    volatile uint32_t *reg = (uint32_t *)((uint32_t)base + ((uint32_t)peripheral >> 16));
    uint32_t mask          = (((uint32_t)peripheral & 0xFF00U) >> 8) - 1;
    uint32_t shift         = (uint32_t)peripheral & 0xFF;

    *reg = (*reg & (~(mask << shift))) | ((accessControl & mask) << shift);
}
