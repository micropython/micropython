/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dmamux.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.dmamux"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get instance number for DMAMUX.
 *
 * @param base DMAMUX peripheral base address.
 */
static uint32_t DMAMUX_GetInstance(DMAMUX_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Array to map DMAMUX instance number to base pointer. */
static DMAMUX_Type *const s_dmamuxBases[] = DMAMUX_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Array to map DMAMUX instance number to clock name. */
static const clock_ip_name_t s_dmamuxClockName[] = DMAMUX_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t DMAMUX_GetInstance(DMAMUX_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_dmamuxBases); instance++)
    {
        if (s_dmamuxBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_dmamuxBases));

    return instance;
}

/*!
 * brief Initializes the DMAMUX peripheral.
 *
 * This function ungates the DMAMUX clock.
 *
 * param base DMAMUX peripheral base address.
 *
 */
void DMAMUX_Init(DMAMUX_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_EnableClock(s_dmamuxClockName[DMAMUX_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Deinitializes the DMAMUX peripheral.
 *
 * This function gates the DMAMUX clock.
 *
 * param base DMAMUX peripheral base address.
 */
void DMAMUX_Deinit(DMAMUX_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_DisableClock(s_dmamuxClockName[DMAMUX_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}
