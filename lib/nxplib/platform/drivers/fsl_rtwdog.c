/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_rtwdog.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.rtwdog"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * brief Clears the RTWDOG flag.
 *
 * This function clears the RTWDOG status flag.
 *
 * Example to clear an interrupt flag:
 * code
 *   RTWDOG_ClearStatusFlags(wdog_base,kRTWDOG_InterruptFlag);
 * endcode
 * param base        RTWDOG peripheral base address.
 * param mask        The status flags to clear.
 *                    The parameter can be any combination of the following values:
 *                    arg kRTWDOG_InterruptFlag
 */
void RTWDOG_ClearStatusFlags(RTWDOG_Type *base, uint32_t mask)
{
    if (mask & kRTWDOG_InterruptFlag)
    {
        base->CS |= RTWDOG_CS_FLG_MASK;
    }
}

/*!
 * brief Initializes the RTWDOG configuration structure.
 *
 * This function initializes the RTWDOG configuration structure to default values. The default
 * values are:
 * code
 *   rtwdogConfig->enableRtwdog = true;
 *   rtwdogConfig->clockSource = kRTWDOG_ClockSource1;
 *   rtwdogConfig->prescaler = kRTWDOG_ClockPrescalerDivide1;
 *   rtwdogConfig->workMode.enableWait = true;
 *   rtwdogConfig->workMode.enableStop = false;
 *   rtwdogConfig->workMode.enableDebug = false;
 *   rtwdogConfig->testMode = kRTWDOG_TestModeDisabled;
 *   rtwdogConfig->enableUpdate = true;
 *   rtwdogConfig->enableInterrupt = false;
 *   rtwdogConfig->enableWindowMode = false;
 *   rtwdogConfig->windowValue = 0U;
 *   rtwdogConfig->timeoutValue = 0xFFFFU;
 * endcode
 *
 * param config Pointer to the RTWDOG configuration structure.
 * see rtwdog_config_t
 */
void RTWDOG_GetDefaultConfig(rtwdog_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enableRtwdog         = true;
    config->clockSource          = kRTWDOG_ClockSource1;
    config->prescaler            = kRTWDOG_ClockPrescalerDivide1;
    config->workMode.enableWait  = true;
    config->workMode.enableStop  = false;
    config->workMode.enableDebug = false;
    config->testMode             = kRTWDOG_TestModeDisabled;
    config->enableUpdate         = true;
    config->enableInterrupt      = false;
    config->enableWindowMode     = false;
    config->windowValue          = 0U;
    config->timeoutValue         = 0xFFFFU;
}

/*!
 * brief Initializes the RTWDOG module.
 *
 * This function initializes the RTWDOG.
 * To reconfigure the RTWDOG without forcing a reset first, enableUpdate must be set to true
 * in the configuration.
 *
 * Example:
 * code
 *   rtwdog_config_t config;
 *   RTWDOG_GetDefaultConfig(&config);
 *   config.timeoutValue = 0x7ffU;
 *   config.enableUpdate = true;
 *   RTWDOG_Init(wdog_base,&config);
 * endcode
 *
 * param base   RTWDOG peripheral base address.
 * param config The configuration of the RTWDOG.
 */
void RTWDOG_Init(RTWDOG_Type *base, const rtwdog_config_t *config)
{
    assert(config);

    uint32_t value        = 0U;
    uint32_t primaskValue = 0U;

    value = RTWDOG_CS_EN(config->enableRtwdog) | RTWDOG_CS_CLK(config->clockSource) |
            RTWDOG_CS_INT(config->enableInterrupt) | RTWDOG_CS_WIN(config->enableWindowMode) |
            RTWDOG_CS_UPDATE(config->enableUpdate) | RTWDOG_CS_DBG(config->workMode.enableDebug) |
            RTWDOG_CS_STOP(config->workMode.enableStop) | RTWDOG_CS_WAIT(config->workMode.enableWait) |
            RTWDOG_CS_PRES(config->prescaler) | RTWDOG_CS_CMD32EN(true) | RTWDOG_CS_TST(config->testMode);

    /* Disable the global interrupts. Otherwise, an interrupt could effectively invalidate the unlock sequence
     * and the WCT may expire. After the configuration finishes, re-enable the global interrupts. */
    primaskValue = DisableGlobalIRQ();
    RTWDOG_Unlock(base);
    base->WIN   = config->windowValue;
    base->TOVAL = config->timeoutValue;
    base->CS    = value;
    while ((base->CS & RTWDOG_CS_RCS_MASK) == 0)
    {
    }
    EnableGlobalIRQ(primaskValue);
}

/*!
 * brief De-initializes the RTWDOG module.
 *
 * This function shuts down the RTWDOG.
 * Ensure that the WDOG_CS.UPDATE is 1, which means that the register update is enabled.
 *
 * param base   RTWDOG peripheral base address.
 */
void RTWDOG_Deinit(RTWDOG_Type *base)
{
    uint32_t primaskValue = 0U;

    /* Disable the global interrupts */
    primaskValue = DisableGlobalIRQ();
    RTWDOG_Unlock(base);
    RTWDOG_Disable(base);
    EnableGlobalIRQ(primaskValue);
}
