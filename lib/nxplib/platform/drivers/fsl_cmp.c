/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_cmp.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.cmp"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for CMP module.
 *
 * @param base CMP peripheral base address
 */
static uint32_t CMP_GetInstance(CMP_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to CMP bases for each instance. */
static CMP_Type *const s_cmpBases[] = CMP_BASE_PTRS;
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to CMP clocks for each instance. */
static const clock_ip_name_t s_cmpClocks[] = CMP_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Codes
 ******************************************************************************/
static uint32_t CMP_GetInstance(CMP_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_cmpBases); instance++)
    {
        if (s_cmpBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_cmpBases));

    return instance;
}

/*!
 * brief Initializes the CMP.
 *
 * This function initializes the CMP module. The operations included are as follows.
 * - Enabling the clock for CMP module.
 * - Configuring the comparator.
 * - Enabling the CMP module.
 * Note that for some devices, multiple CMP instances share the same clock gate. In this case, to enable the clock for
 * any instance enables all CMPs. See the appropriate MCU reference manual for the clock assignment of the CMP.
 *
 * param base   CMP peripheral base address.
 * param config Pointer to the configuration structure.
 */
void CMP_Init(CMP_Type *base, const cmp_config_t *config)
{
    assert(NULL != config);

    uint8_t tmp8;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the clock. */
    CLOCK_EnableClock(s_cmpClocks[CMP_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Configure. */
    CMP_Enable(base, false); /* Disable the CMP module during configuring. */
    /* CMPx_CR1. */
    tmp8 = (uint8_t)(base->CR1 & ~(CMP_CR1_PMODE_MASK | CMP_CR1_INV_MASK | CMP_CR1_COS_MASK | CMP_CR1_OPE_MASK));
    if (true == config->enableHighSpeed)
    {
        tmp8 |= CMP_CR1_PMODE_MASK;
    }
    if (true == config->enableInvertOutput)
    {
        tmp8 |= CMP_CR1_INV_MASK;
    }
    if (true == config->useUnfilteredOutput)
    {
        tmp8 |= CMP_CR1_COS_MASK;
    }
    if (true == config->enablePinOut)
    {
        tmp8 |= CMP_CR1_OPE_MASK;
    }
#if defined(FSL_FEATURE_CMP_HAS_TRIGGER_MODE) && FSL_FEATURE_CMP_HAS_TRIGGER_MODE
    if (true == config->enableTriggerMode)
    {
        tmp8 |= CMP_CR1_TRIGM_MASK;
    }
    else
    {
        tmp8 &= ~(uint8_t)CMP_CR1_TRIGM_MASK;
    }
#endif /* FSL_FEATURE_CMP_HAS_TRIGGER_MODE */
    base->CR1 = tmp8;

    /* CMPx_CR0. */
    tmp8 = base->CR0 & ~(uint8_t)CMP_CR0_HYSTCTR_MASK;
    tmp8 |= CMP_CR0_HYSTCTR(config->hysteresisMode);
    base->CR0 = tmp8;

    CMP_Enable(base, config->enableCmp); /* Enable the CMP module after configured or not. */
}

/*!
 * brief De-initializes the CMP module.
 *
 * This function de-initializes the CMP module. The operations included are as follows.
 * - Disabling the CMP module.
 * - Disabling the clock for CMP module.
 *
 * This function disables the clock for the CMP.
 * Note that for some devices, multiple CMP instances share the same clock gate. In this case, before disabling the
 * clock for the CMP, ensure that all the CMP instances are not used.
 *
 * param base CMP peripheral base address.
 */
void CMP_Deinit(CMP_Type *base)
{
    /* Disable the CMP module. */
    CMP_Enable(base, false);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable the clock. */
    CLOCK_DisableClock(s_cmpClocks[CMP_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Initializes the CMP user configuration structure.
 *
 * This function initializes the user configuration structure to these default values.
 * code
 *   config->enableCmp           = true;
 *   config->hysteresisMode      = kCMP_HysteresisLevel0;
 *   config->enableHighSpeed     = false;
 *   config->enableInvertOutput  = false;
 *   config->useUnfilteredOutput = false;
 *   config->enablePinOut        = false;
 *   config->enableTriggerMode   = false;
 * endcode
 * param config Pointer to the configuration structure.
 */
void CMP_GetDefaultConfig(cmp_config_t *config)
{
    assert(NULL != config);

    /* Initializes the configure structure to zero. */
    (void)memset(config, 0, sizeof(*config));

    config->enableCmp           = true; /* Enable the CMP module after initialization. */
    config->hysteresisMode      = kCMP_HysteresisLevel0;
    config->enableHighSpeed     = false;
    config->enableInvertOutput  = false;
    config->useUnfilteredOutput = false;
    config->enablePinOut        = false;
#if defined(FSL_FEATURE_CMP_HAS_TRIGGER_MODE) && FSL_FEATURE_CMP_HAS_TRIGGER_MODE
    config->enableTriggerMode = false;
#endif /* FSL_FEATURE_CMP_HAS_TRIGGER_MODE */
}

/*!
 * brief  Sets the input channels for the comparator.
 *
 * This function sets the input channels for the comparator.
 * Note that two input channels cannot be set the same way in the application. When the user selects the same input
 * from the analog mux to the positive and negative port, the comparator is disabled automatically.
 *
 * param  base            CMP peripheral base address.
 * param  positiveChannel Positive side input channel number. Available range is 0-7.
 * param  negativeChannel Negative side input channel number. Available range is 0-7.
 */
void CMP_SetInputChannels(CMP_Type *base, uint8_t positiveChannel, uint8_t negativeChannel)
{
    uint8_t tmp8 = base->MUXCR;

    tmp8 &= ~(uint8_t)(CMP_MUXCR_PSEL_MASK | CMP_MUXCR_MSEL_MASK);
    tmp8 |= CMP_MUXCR_PSEL(positiveChannel) | CMP_MUXCR_MSEL(negativeChannel);
    base->MUXCR = tmp8;
}

#if defined(FSL_FEATURE_CMP_HAS_DMA) && FSL_FEATURE_CMP_HAS_DMA
/*!
 * brief Enables/disables the DMA request for rising/falling events.
 *
 * This function enables/disables the DMA request for rising/falling events. Either event triggers the generation of
 * the DMA request from CMP if the DMA feature is enabled. Both events are ignored for generating the DMA request from
 * the CMP
 * if the DMA is disabled.
 *
 * param base CMP peripheral base address.
 * param enable Enables or disables the feature.
 */
void CMP_EnableDMA(CMP_Type *base, bool enable)
{
    uint8_t tmp8 = (uint8_t)(base->SCR & ~(CMP_SCR_CFR_MASK | CMP_SCR_CFF_MASK)); /* To avoid change the w1c bits. */

    if (enable)
    {
        tmp8 |= CMP_SCR_DMAEN_MASK;
    }
    else
    {
        tmp8 &= ~(uint8_t)CMP_SCR_DMAEN_MASK;
    }
    base->SCR = tmp8;
}
#endif /* FSL_FEATURE_CMP_HAS_DMA */

/*!
 * brief  Configures the filter.
 *
 * param  base   CMP peripheral base address.
 * param  config Pointer to the configuration structure.
 */
void CMP_SetFilterConfig(CMP_Type *base, const cmp_filter_config_t *config)
{
    assert(NULL != config);

    uint8_t tmp8;

#if defined(FSL_FEATURE_CMP_HAS_EXTERNAL_SAMPLE_SUPPORT) && FSL_FEATURE_CMP_HAS_EXTERNAL_SAMPLE_SUPPORT
    /* Choose the clock source for sampling. */
    if (config->enableSample)
    {
        base->CR1 |= CMP_CR1_SE_MASK; /* Choose the external SAMPLE clock. */
    }
    else
    {
        base->CR1 &= ~CMP_CR1_SE_MASK; /* Choose the internal divided bus clock. */
    }
#endif /* FSL_FEATURE_CMP_HAS_EXTERNAL_SAMPLE_SUPPORT */
    /* Set the filter count. */
    tmp8 = (uint8_t)(base->CR0 & ~CMP_CR0_FILTER_CNT_MASK);
    tmp8 |= CMP_CR0_FILTER_CNT(config->filterCount);
    base->CR0 = tmp8;
    /* Set the filter period. It is used as the divider to bus clock. */
    base->FPR = CMP_FPR_FILT_PER(config->filterPeriod);
}

/*!
 * brief Configures the internal DAC.
 *
 * param base   CMP peripheral base address.
 * param config Pointer to the configuration structure. "NULL" disables the feature.
 */
void CMP_SetDACConfig(CMP_Type *base, const cmp_dac_config_t *config)
{
    uint8_t tmp8 = 0U;

    if (NULL == config)
    {
        /* Passing "NULL" as input parameter means no available configuration. So the DAC feature is disabled.*/
        base->DACCR = 0U;
        return;
    }
    /* CMPx_DACCR. */
    tmp8 |= CMP_DACCR_DACEN_MASK; /* Enable the internal DAC. */
    if (kCMP_VrefSourceVin2 == config->referenceVoltageSource)
    {
        tmp8 |= CMP_DACCR_VRSEL_MASK;
    }
    tmp8 |= CMP_DACCR_VOSEL(config->DACValue);

    base->DACCR = tmp8;
}

/*!
 * brief Enables the interrupts.
 *
 * param base    CMP peripheral base address.
 * param mask    Mask value for interrupts. See "_cmp_interrupt_enable".
 */
void CMP_EnableInterrupts(CMP_Type *base, uint32_t mask)
{
    uint8_t tmp8 = (uint8_t)(base->SCR & ~(CMP_SCR_CFR_MASK | CMP_SCR_CFF_MASK)); /* To avoid change the w1c bits. */

    if (0U != ((uint32_t)kCMP_OutputRisingInterruptEnable & mask))
    {
        tmp8 |= CMP_SCR_IER_MASK;
    }
    if (0U != ((uint32_t)kCMP_OutputFallingInterruptEnable & mask))
    {
        tmp8 |= CMP_SCR_IEF_MASK;
    }
    base->SCR = tmp8;
}

/*!
 * brief Disables the interrupts.
 *
 * param base    CMP peripheral base address.
 * param mask    Mask value for interrupts. See "_cmp_interrupt_enable".
 */
void CMP_DisableInterrupts(CMP_Type *base, uint32_t mask)
{
    uint8_t tmp8 = (uint8_t)(base->SCR & ~(CMP_SCR_CFR_MASK | CMP_SCR_CFF_MASK)); /* To avoid change the w1c bits. */

    if (0U != ((uint32_t)kCMP_OutputRisingInterruptEnable & mask))
    {
        tmp8 &= ~(uint8_t)CMP_SCR_IER_MASK;
    }
    if (0U != ((uint32_t)kCMP_OutputFallingInterruptEnable & mask))
    {
        tmp8 &= ~(uint8_t)CMP_SCR_IEF_MASK;
    }
    base->SCR = tmp8;
}

/*!
 * brief  Gets the status flags.
 *
 * param  base     CMP peripheral base address.
 *
 * return          Mask value for the asserted flags. See "_cmp_status_flags".
 */
uint32_t CMP_GetStatusFlags(CMP_Type *base)
{
    uint32_t ret32 = 0U;

    if (0U != (CMP_SCR_CFR_MASK & base->SCR))
    {
        ret32 |= (uint32_t)kCMP_OutputRisingEventFlag;
    }
    if (0U != (CMP_SCR_CFF_MASK & base->SCR))
    {
        ret32 |= (uint32_t)kCMP_OutputFallingEventFlag;
    }
    if (0U != (CMP_SCR_COUT_MASK & base->SCR))
    {
        ret32 |= (uint32_t)kCMP_OutputAssertEventFlag;
    }
    return ret32;
}

/*!
 * brief Clears the status flags.
 *
 * param base     CMP peripheral base address.
 * param mask     Mask value for the flags. See "_cmp_status_flags".
 */
void CMP_ClearStatusFlags(CMP_Type *base, uint32_t mask)
{
    uint8_t tmp8 = (uint8_t)(base->SCR & ~(CMP_SCR_CFR_MASK | CMP_SCR_CFF_MASK)); /* To avoid change the w1c bits. */

    if (0U != ((uint32_t)kCMP_OutputRisingEventFlag & mask))
    {
        tmp8 |= CMP_SCR_CFR_MASK;
    }
    if (0U != ((uint32_t)kCMP_OutputFallingEventFlag & mask))
    {
        tmp8 |= CMP_SCR_CFF_MASK;
    }
    base->SCR = tmp8;
}
