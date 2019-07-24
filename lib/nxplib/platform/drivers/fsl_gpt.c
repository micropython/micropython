/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_gpt.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.gpt"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to GPT bases for each instance. */
static GPT_Type *const s_gptBases[] = GPT_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to GPT clocks for each instance. */
static const clock_ip_name_t s_gptClocks[] = GPT_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t GPT_GetInstance(GPT_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0U; instance < ARRAY_SIZE(s_gptBases); instance++)
    {
        if (s_gptBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_gptBases));

    return instance;
}

/*!
 * brief Initialize GPT to reset state and initialize running mode.
 *
 * param base GPT peripheral base address.
 * param initConfig GPT mode setting configuration.
 */
void GPT_Init(GPT_Type *base, const gpt_config_t *initConfig)
{
    assert(initConfig);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the GPT clock*/
    CLOCK_EnableClock(s_gptClocks[GPT_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    base->CR = 0U;

    GPT_SoftwareReset(base);

    base->CR =
        (initConfig->enableFreeRun ? GPT_CR_FRR_MASK : 0U) | (initConfig->enableRunInWait ? GPT_CR_WAITEN_MASK : 0U) |
        (initConfig->enableRunInStop ? GPT_CR_STOPEN_MASK : 0U) |
        (initConfig->enableRunInDoze ? GPT_CR_DOZEEN_MASK : 0U) |
        (initConfig->enableRunInDbg ? GPT_CR_DBGEN_MASK : 0U) | (initConfig->enableMode ? GPT_CR_ENMOD_MASK : 0U);

    GPT_SetClockSource(base, initConfig->clockSource);
    GPT_SetClockDivider(base, initConfig->divider);
}

/*!
 * brief Disables the module and gates the GPT clock.
 *
 * param base GPT peripheral base address.
 */
void GPT_Deinit(GPT_Type *base)
{
    /* Disable GPT timers */
    base->CR = 0U;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Gate the GPT clock*/
    CLOCK_DisableClock(s_gptClocks[GPT_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Fills in the GPT configuration structure with default settings.
 *
 * The default values are:
 * code
 *    config->clockSource = kGPT_ClockSource_Periph;
 *    config->divider = 1U;
 *    config->enableRunInStop = true;
 *    config->enableRunInWait = true;
 *    config->enableRunInDoze = false;
 *    config->enableRunInDbg = false;
 *    config->enableFreeRun = true;
 *    config->enableMode  = true;
 * endcode
 * param config Pointer to the user configuration structure.
 */
void GPT_GetDefaultConfig(gpt_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->clockSource     = kGPT_ClockSource_Periph;
    config->divider         = 1U;
    config->enableRunInStop = true;
    config->enableRunInWait = true;
    config->enableRunInDoze = false;
    config->enableRunInDbg  = false;
    config->enableFreeRun   = false;
    config->enableMode      = true;
}
