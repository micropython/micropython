/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ewm.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.ewm"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * brief Initializes the EWM peripheral.
 *
 * This function is used to initialize the EWM. After calling, the EWM
 * runs immediately according to the configuration.
 * Note that, except for the interrupt enable control bit, other control bits and registers are write once after a
 * CPU reset. Modifying them more than once generates a bus transfer error.
 *
 * This is an example.
 * code
 *   ewm_config_t config;
 *   EWM_GetDefaultConfig(&config);
 *   config.compareHighValue = 0xAAU;
 *   EWM_Init(ewm_base,&config);
 * endcode
 *
 * param base EWM peripheral base address
 * param config The configuration of the EWM
 */
void EWM_Init(EWM_Type *base, const ewm_config_t *config)
{
    assert(config);

    uint32_t value = 0U;

#if !((defined(FSL_FEATURE_SOC_PCC_COUNT) && FSL_FEATURE_SOC_PCC_COUNT) && \
      (defined(FSL_FEATURE_PCC_SUPPORT_EWM_CLOCK_REMOVE) && FSL_FEATURE_PCC_SUPPORT_EWM_CLOCK_REMOVE))
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_EnableClock(kCLOCK_Ewm0);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
#endif
    value = EWM_CTRL_EWMEN(config->enableEwm) | EWM_CTRL_ASSIN(config->setInputAssertLogic) |
            EWM_CTRL_INEN(config->enableEwmInput) | EWM_CTRL_INTEN(config->enableInterrupt);
#if defined(FSL_FEATURE_EWM_HAS_PRESCALER) && FSL_FEATURE_EWM_HAS_PRESCALER
    base->CLKPRESCALER = config->prescaler;
#endif /* FSL_FEATURE_EWM_HAS_PRESCALER */

#if defined(FSL_FEATURE_EWM_HAS_CLOCK_SELECT) && FSL_FEATURE_EWM_HAS_CLOCK_SELECT
    base->CLKCTRL = config->clockSource;
#endif /* FSL_FEATURE_EWM_HAS_CLOCK_SELECT*/

    base->CMPL = config->compareLowValue;
    base->CMPH = config->compareHighValue;
    base->CTRL = value;
}

/*!
 * brief Deinitializes the EWM peripheral.
 *
 * This function is used to shut down the EWM.
 *
 * param base EWM peripheral base address
 */
void EWM_Deinit(EWM_Type *base)
{
    EWM_DisableInterrupts(base, kEWM_InterruptEnable);
#if !((defined(FSL_FEATURE_SOC_PCC_COUNT) && FSL_FEATURE_SOC_PCC_COUNT) && \
      (defined(FSL_FEATURE_PCC_SUPPORT_EWM_CLOCK_REMOVE) && FSL_FEATURE_PCC_SUPPORT_EWM_CLOCK_REMOVE))
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_DisableClock(kCLOCK_Ewm0);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
#endif /* FSL_FEATURE_PCC_SUPPORT_EWM_CLOCK_REMOVE */
}

/*!
 * brief Initializes the EWM configuration structure.
 *
 * This function initializes the EWM configuration structure to default values. The default
 * values are as follows.
 * code
 *   ewmConfig->enableEwm = true;
 *   ewmConfig->enableEwmInput = false;
 *   ewmConfig->setInputAssertLogic = false;
 *   ewmConfig->enableInterrupt = false;
 *   ewmConfig->ewm_lpo_clock_source_t = kEWM_LpoClockSource0;
 *   ewmConfig->prescaler = 0;
 *   ewmConfig->compareLowValue = 0;
 *   ewmConfig->compareHighValue = 0xFEU;
 * endcode
 *
 * param config Pointer to the EWM configuration structure.
 * see ewm_config_t
 */
void EWM_GetDefaultConfig(ewm_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enableEwm           = true;
    config->enableEwmInput      = false;
    config->setInputAssertLogic = false;
    config->enableInterrupt     = false;
#if defined(FSL_FEATURE_EWM_HAS_CLOCK_SELECT) && FSL_FEATURE_EWM_HAS_CLOCK_SELECT
    config->clockSource = kEWM_LpoClockSource0;
#endif /* FSL_FEATURE_EWM_HAS_CLOCK_SELECT*/
#if defined(FSL_FEATURE_EWM_HAS_PRESCALER) && FSL_FEATURE_EWM_HAS_PRESCALER
    config->prescaler = 0U;
#endif /* FSL_FEATURE_EWM_HAS_PRESCALER */
    config->compareLowValue  = 0U;
    config->compareHighValue = 0xFEU;
}

/*!
 * brief Services the EWM.
 *
 * This function resets the EWM counter to zero.
 *
 * param base EWM peripheral base address
 */
void EWM_Refresh(EWM_Type *base)
{
    uint32_t primaskValue = 0U;

    /* Disable the global interrupt to protect refresh sequence */
    primaskValue = DisableGlobalIRQ();
    base->SERV   = (uint8_t)0xB4U;
    base->SERV   = (uint8_t)0x2CU;
    EnableGlobalIRQ(primaskValue);
}
