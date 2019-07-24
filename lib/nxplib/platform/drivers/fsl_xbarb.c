/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_xbarb.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.xbarb"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get the XBARB instance from peripheral base address.
 *
 * @param base XBARB peripheral base address.
 * @return XBARB instance.
 */
static uint32_t XBARB_GetInstance(XBARB_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array of XBARB peripheral base address. */
static XBARB_Type *const s_xbarbBases[] = XBARB_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Array of XBARB clock name. */
static const clock_ip_name_t s_xbarbClock[] = XBARB_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/

static uint32_t XBARB_GetInstance(XBARB_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_xbarbBases); instance++)
    {
        if (s_xbarbBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_xbarbBases));

    return instance;
}

/*!
 * brief Initializes the XBARB module.
 *
 * This function un-gates the XBARB clock.
 *
 * param base XBARB peripheral address.
 */
void XBARB_Init(XBARB_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable XBARB module clock. */
    CLOCK_EnableClock(s_xbarbClock[XBARB_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Shuts down the XBARB module.
 *
 * This function disables XBARB clock.
 *
 * param base XBARB peripheral address.
 */
void XBARB_Deinit(XBARB_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable XBARB module clock. */
    CLOCK_DisableClock(s_xbarbClock[XBARB_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Configures a connection between the selected XBARB_IN[*] input and the XBARB_OUT[*] output signal.
 *
 * This function configures which XBARB input is connected to the selected XBARB output.
 * If more than one XBARB module is available, only the inputs and outputs from the same module
 * can be connected.
 *
 * param base XBARB peripheral address.
 * param input XBARB input signal.
 * param output XBARB output signal.
 */
void XBARB_SetSignalsConnection(XBARB_Type *base, xbar_input_signal_t input, xbar_output_signal_t output)
{
    XBARB_WR_SELx_SELx(base, (((uint16_t)input) & 0xFFU), (((uint16_t)output) & 0xFFU));
}
