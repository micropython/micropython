/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_wdog.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.wdog01"
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
static WDOG_Type *const s_wdogBases[] = WDOG_BASE_PTRS;
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Array of WDOG clock name. */
static const clock_ip_name_t s_wdogClock[] = WDOG_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

static const IRQn_Type s_wdogIRQ[] = WDOG_IRQS;

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t WDOG_GetInstance(WDOG_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_wdogBases); instance++)
    {
        if (s_wdogBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_wdogBases));

    return instance;
}

/*!
 * brief Initializes the WDOG configuration structure.
 *
 * This function initializes the WDOG configuration structure to default values. The default
 * values are as follows.
 * code
 *   wdogConfig->enableWdog = true;
 *   wdogConfig->workMode.enableWait = true;
 *   wdogConfig->workMode.enableStop = false;
 *   wdogConfig->workMode.enableDebug = false;
 *   wdogConfig->enableInterrupt = false;
 *   wdogConfig->enablePowerdown = false;
 *   wdogConfig->resetExtension = flase;
 *   wdogConfig->timeoutValue = 0xFFU;
 *   wdogConfig->interruptTimeValue = 0x04u;
 * endcode
 *
 * param config Pointer to the WDOG configuration structure.
 * see wdog_config_t
 */
void WDOG_GetDefaultConfig(wdog_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enableWdog             = true;
    config->workMode.enableWait    = false;
    config->workMode.enableStop    = false;
    config->workMode.enableDebug   = false;
    config->enableInterrupt        = false;
    config->softwareResetExtension = false;
    config->enablePowerDown        = false;
    config->timeoutValue           = 0xffu;
    config->interruptTimeValue     = 0x04u;
    config->enableTimeOutAssert    = false;
}

/*!
 * brief Initializes the WDOG.
 *
 * This function initializes the WDOG. When called, the WDOG runs according to the configuration.
 *
 * This is an example.
 * code
 *   wdog_config_t config;
 *   WDOG_GetDefaultConfig(&config);
 *   config.timeoutValue = 0xffU;
 *   config->interruptTimeValue = 0x04u;
 *   WDOG_Init(wdog_base,&config);
 * endcode
 *
 * param base   WDOG peripheral base address
 * param config The configuration of WDOG
 */
void WDOG_Init(WDOG_Type *base, const wdog_config_t *config)
{
    assert(config);

    uint16_t value = 0u;

    value = WDOG_WCR_WDE(config->enableWdog) | WDOG_WCR_WDW(config->workMode.enableWait) |
            WDOG_WCR_WDZST(config->workMode.enableStop) | WDOG_WCR_WDBG(config->workMode.enableDebug) |
            WDOG_WCR_SRE(config->softwareResetExtension) | WDOG_WCR_WT(config->timeoutValue) |
            WDOG_WCR_WDT(config->enableTimeOutAssert) | WDOG_WCR_SRS_MASK | WDOG_WCR_WDA_MASK;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Set configuration */
    CLOCK_EnableClock(s_wdogClock[WDOG_GetInstance(base)]);
#endif

    base->WICR = WDOG_WICR_WICT(config->interruptTimeValue) | WDOG_WICR_WIE(config->enableInterrupt);
    base->WMCR = WDOG_WMCR_PDE(config->enablePowerDown);
    base->WCR  = value;
    if (config->enableInterrupt)
    {
        EnableIRQ(s_wdogIRQ[WDOG_GetInstance(base)]);
    }
}

/*!
 * brief Shuts down the WDOG.
 *
 * This function shuts down the WDOG.
 * Watchdog Enable bit is a write one once only bit. It is not
 * possible to clear this bit by a software write, once the bit is set.
 * This bit(WDE) can be set/reset only in debug mode(exception).
 */
void WDOG_Deinit(WDOG_Type *base)
{
    if (base->WCR & WDOG_WCR_WDBG_MASK)
    {
        WDOG_Disable(base);
    }
}

/*!
 * brief Gets the WDOG all reset status flags.
 *
 * This function gets all reset status flags.
 *
 * code
 * uint16_t status;
 * status = WDOG_GetStatusFlags (wdog_base);
 * endcode
 * param base        WDOG peripheral base address
 * return            State of the status flag: asserted (true) or not-asserted (false).see _wdog_status_flags
 *                    - true: a related status flag has been set.
 *                    - false: a related status flag is not set.
 */
uint16_t WDOG_GetStatusFlags(WDOG_Type *base)
{
    uint16_t status_flag = 0U;

    status_flag |= (base->WCR & WDOG_WCR_WDE_MASK);
    status_flag |= (base->WRSR & WDOG_WRSR_POR_MASK);
    status_flag |= (base->WRSR & WDOG_WRSR_TOUT_MASK);
    status_flag |= (base->WRSR & WDOG_WRSR_SFTW_MASK);
    status_flag |= (base->WICR & WDOG_WICR_WTIS_MASK);

    return status_flag;
}

/*!
 * brief Clears the WDOG flag.
 *
 * This function clears the WDOG status flag.
 *
 * This is an example for clearing the interrupt flag.
 * code
 *   WDOG_ClearStatusFlags(wdog_base,KWDOG_InterruptFlag);
 * endcode
 * param base        WDOG peripheral base address
 * param mask        The status flags to clear.
 *                    The parameter could be any combination of the following values.
 *                    kWDOG_TimeoutFlag
 */
void WDOG_ClearInterruptStatus(WDOG_Type *base, uint16_t mask)
{
    if (mask & kWDOG_InterruptFlag)
    {
        base->WICR |= WDOG_WICR_WTIS_MASK;
    }
}

/*!
 * brief Refreshes the WDOG timer.
 *
 * This function feeds the WDOG.
 * This function should be called before the WDOG timer is in timeout. Otherwise, a reset is asserted.
 *
 * param base WDOG peripheral base address
 */
void WDOG_Refresh(WDOG_Type *base)
{
    base->WSR = WDOG_REFRESH_KEY & 0xFFFFU;
    base->WSR = (WDOG_REFRESH_KEY >> 16U) & 0xFFFFU;
}
