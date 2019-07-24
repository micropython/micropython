/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_src.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.src"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * brief Clear the status flags of SRC.
 *
 * param base SRC peripheral base address.
 * param Mask value of status flags to be cleared, see to #_src_reset_status_flags.
 */
void SRC_ClearResetStatusFlags(SRC_Type *base, uint32_t flags)
{
    uint32_t tmp32 = base->SRSR;

    if (0U != (SRC_SRSR_TSR_MASK & flags))
    {
        tmp32 &= ~SRC_SRSR_TSR_MASK; /* Write 0 to clear. */
    }

    if (0U != (SRC_SRSR_W1C_BITS_MASK & flags))
    {
        tmp32 |= (SRC_SRSR_W1C_BITS_MASK & flags); /* Write 1 to clear. */
    }

    base->SRSR = tmp32;
}
