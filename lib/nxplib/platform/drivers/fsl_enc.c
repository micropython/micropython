/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_enc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.enc"
#endif

#define ENC_CTRL_W1C_FLAGS (ENC_CTRL_HIRQ_MASK | ENC_CTRL_XIRQ_MASK | ENC_CTRL_DIRQ_MASK | ENC_CTRL_CMPIRQ_MASK)
#define ENC_CTRL2_W1C_FLAGS (ENC_CTRL2_SABIRQ_MASK | ENC_CTRL2_ROIRQ_MASK | ENC_CTRL2_RUIRQ_MASK)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for ENC module.
 *
 * @param base ENC peripheral base address
 */
static uint32_t ENC_GetInstance(ENC_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to ENC bases for each instance. */
static ENC_Type *const s_encBases[] = ENC_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to ENC clocks for each instance. */
static const clock_ip_name_t s_encClocks[] = ENC_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t ENC_GetInstance(ENC_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_encBases); instance++)
    {
        if (s_encBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_encBases));

    return instance;
}

/*!
 * brief Initialization for the ENC module.
 *
 * This function is to make the initialization for the ENC module. It should be called firstly before any operation to
 * the ENC with the operations like:
 *  - Enable the clock for ENC module.
 *  - Configure the ENC's working attributes.
 *
 * param base   ENC peripheral base address.
 * param config Pointer to configuration structure. See to "enc_config_t".
 */
void ENC_Init(ENC_Type *base, const enc_config_t *config)
{
    assert(NULL != config);

    uint32_t tmp16;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the clock. */
    CLOCK_EnableClock(s_encClocks[ENC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* ENC_CTRL. */
    tmp16 = base->CTRL & (uint16_t)(~(ENC_CTRL_W1C_FLAGS | ENC_CTRL_HIP_MASK | ENC_CTRL_HNE_MASK | ENC_CTRL_REV_MASK |
                                      ENC_CTRL_PH1_MASK | ENC_CTRL_XIP_MASK | ENC_CTRL_XNE_MASK | ENC_CTRL_WDE_MASK));
    /* For HOME trigger. */
    if (kENC_HOMETriggerDisabled != config->HOMETriggerMode)
    {
        tmp16 |= ENC_CTRL_HIP_MASK;
        if (kENC_HOMETriggerOnFallingEdge == config->HOMETriggerMode)
        {
            tmp16 |= ENC_CTRL_HNE_MASK;
        }
    }
    /* For encoder work mode. */
    if (config->enableReverseDirection)
    {
        tmp16 |= ENC_CTRL_REV_MASK;
    }
    if (kENC_DecoderWorkAsSignalPhaseCountMode == config->decoderWorkMode)
    {
        tmp16 |= ENC_CTRL_PH1_MASK;
    }
    /* For INDEX trigger. */
    if (kENC_INDEXTriggerDisabled != config->INDEXTriggerMode)
    {
        tmp16 |= ENC_CTRL_XIP_MASK;
        if (kENC_INDEXTriggerOnFallingEdge == config->INDEXTriggerMode)
        {
            tmp16 |= ENC_CTRL_XNE_MASK;
        }
    }
    /* Watchdog. */
    if (config->enableWatchdog)
    {
        tmp16 |= ENC_CTRL_WDE_MASK;
        base->WTR = config->watchdogTimeoutValue; /* WDOG can be only available when the feature is enabled. */
    }
    base->CTRL = tmp16;

    /* ENC_FILT. */
    base->FILT = ENC_FILT_FILT_CNT(config->filterCount) | ENC_FILT_FILT_PER(config->filterSamplePeriod);

    /* ENC_CTRL2. */
    tmp16 = base->CTRL2 & (uint16_t)(~(ENC_CTRL2_W1C_FLAGS | ENC_CTRL2_OUTCTL_MASK | ENC_CTRL2_REVMOD_MASK |
                                       ENC_CTRL2_MOD_MASK | ENC_CTRL2_UPDPOS_MASK | ENC_CTRL2_UPDHLD_MASK));
    if (kENC_POSMATCHOnReadingAnyPositionCounter == config->positionMatchMode)
    {
        tmp16 |= ENC_CTRL2_OUTCTL_MASK;
    }
    if (kENC_RevolutionCountOnRollOverModulus == config->revolutionCountCondition)
    {
        tmp16 |= ENC_CTRL2_REVMOD_MASK;
    }
    if (config->enableModuloCountMode)
    {
        tmp16 |= ENC_CTRL2_MOD_MASK;
        /* Set modulus value. */
        base->UMOD = (uint16_t)(config->positionModulusValue >> 16U); /* Upper 16 bits. */
        base->LMOD = (uint16_t)(config->positionModulusValue);        /* Lower 16 bits. */
    }
    if (config->enableTRIGGERClearPositionCounter)
    {
        tmp16 |= ENC_CTRL2_UPDPOS_MASK;
    }
    if (config->enableTRIGGERClearHoldPositionCounter)
    {
        tmp16 |= ENC_CTRL2_UPDHLD_MASK;
    }
    base->CTRL2 = tmp16;

    /* ENC_UCOMP & ENC_LCOMP. */
    base->UCOMP = (uint16_t)(config->positionCompareValue >> 16U); /* Upper 16 bits. */
    base->LCOMP = (uint16_t)(config->positionCompareValue);        /* Lower 16 bits. */

    /* ENC_UINIT & ENC_LINIT. */
    base->UINIT = (uint16_t)(config->positionInitialValue >> 16U); /* Upper 16 bits. */
    base->LINIT = (uint16_t)(config->positionInitialValue);        /* Lower 16 bits. */
}

/*!
 * brief De-initialization for the ENC module.
 *
 * This function is to make the de-initialization for the ENC module. It could be called when ENC is no longer used with
 * the operations like:
 *  - Disable the clock for ENC module.
 *
 * param base ENC peripheral base address.
 */
void ENC_Deinit(ENC_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable the clock. */
    CLOCK_DisableClock(s_encClocks[ENC_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Get an available pre-defined settings for ENC's configuration.
 *
 * This function initializes the ENC configuration structure with an available settings, the default value are:
 * code
 *   config->enableReverseDirection                = false;
 *   config->decoderWorkMode                       = kENC_DecoderWorkAsNormalMode;
 *   config->HOMETriggerMode                       = kENC_HOMETriggerDisabled;
 *   config->INDEXTriggerMode                      = kENC_INDEXTriggerDisabled;
 *   config->enableTRIGGERClearPositionCounter     = false;
 *   config->enableTRIGGERClearHoldPositionCounter = false;
 *   config->enableWatchdog                        = false;
 *   config->watchdogTimeoutValue                  = 0U;
 *   config->filterCount                           = 0U;
 *   config->filterSamplePeriod                    = 0U;
 *   config->positionMatchMode                     = kENC_POSMATCHOnPositionCounterEqualToComapreValue;
 *   config->positionCompareValue                  = 0xFFFFFFFFU;
 *   config->revolutionCountCondition              = kENC_RevolutionCountOnINDEXPulse;
 *   config->enableModuloCountMode                 = false;
 *   config->positionModulusValue                  = 0U;
 *   config->positionInitialValue                  = 0U;
 * endcode
 * param config Pointer to a variable of configuration structure. See to "enc_config_t".
 */
void ENC_GetDefaultConfig(enc_config_t *config)
{
    assert(NULL != config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->enableReverseDirection                = false;
    config->decoderWorkMode                       = kENC_DecoderWorkAsNormalMode;
    config->HOMETriggerMode                       = kENC_HOMETriggerDisabled;
    config->INDEXTriggerMode                      = kENC_INDEXTriggerDisabled;
    config->enableTRIGGERClearPositionCounter     = false;
    config->enableTRIGGERClearHoldPositionCounter = false;
    config->enableWatchdog                        = false;
    config->watchdogTimeoutValue                  = 0U;
    config->filterCount                           = 0U;
    config->filterSamplePeriod                    = 0U;
    config->positionMatchMode                     = kENC_POSMATCHOnPositionCounterEqualToComapreValue;
    config->positionCompareValue                  = 0xFFFFFFFFU;
    config->revolutionCountCondition              = kENC_RevolutionCountOnINDEXPulse;
    config->enableModuloCountMode                 = false;
    config->positionModulusValue                  = 0U;
    config->positionInitialValue                  = 0U;
}

/*!
 * brief Load the initial position value to position counter.
 *
 * This function is to transfer the initial position value (UINIT and LINIT) contents to position counter (UPOS and
 * LPOS), so that to provide the consistent operation the position counter registers.
 *
 * param base ENC peripheral base address.
 */
void ENC_DoSoftwareLoadInitialPositionValue(ENC_Type *base)
{
    uint16_t tmp16 = base->CTRL & (uint16_t)(~ENC_CTRL_W1C_FLAGS);

    tmp16 |= ENC_CTRL_SWIP_MASK; /* Write 1 to trigger the command for loading initial position value. */
    base->CTRL = tmp16;
}

/*!
 * brief Enable and configure the self test function.
 *
 * This function is to enable and configuration the self test function. It controls and sets the frequency of a
 * quadrature signal generator. It provides a quadrature test signal to the inputs of the quadrature decoder module.
 * It is a factory test feature; however, it may be useful to customers' software development and testing.
 *
 * param base   ENC peripheral base address.
 * param config Pointer to configuration structure. See to "enc_self_test_config_t". Pass "NULL" to disable.
 */
void ENC_SetSelfTestConfig(ENC_Type *base, const enc_self_test_config_t *config)
{
    uint16_t tmp16 = 0U;

    if (NULL == config) /* Pass "NULL" to disable the feature. */
    {
        base->TST = 0U;
        return;
    }
    tmp16 = ENC_TST_TEN_MASK | ENC_TST_TCE_MASK | ENC_TST_TEST_PERIOD(config->signalPeriod) |
            ENC_TST_TEST_COUNT(config->signalCount);
    if (kENC_SelfTestDirectionNegative == config->signalDirection)
    {
        tmp16 |= ENC_TST_QDN_MASK;
    }
    base->TST = tmp16;
}

/*!
 * brief Enable watchdog for ENC module.
 *
 * param base ENC peripheral base address
 * param enable Enables or disables the watchdog
 */
void ENC_EnableWatchdog(ENC_Type *base, bool enable)
{
    uint16_t tmp16 = base->CTRL & (uint16_t)(~(ENC_CTRL_W1C_FLAGS | ENC_CTRL_WDE_MASK));

    if (enable)
    {
        tmp16 |= ENC_CTRL_WDE_MASK;
    }
    base->CTRL = tmp16;
}

/*!
 * brief  Get the status flags.
 *
 * param  base ENC peripheral base address.
 *
 * return      Mask value of status flags. For available mask, see to "_enc_status_flags".
 */
uint32_t ENC_GetStatusFlags(ENC_Type *base)
{
    uint32_t ret32 = 0U;

    /* ENC_CTRL. */
    if (ENC_CTRL_HIRQ_MASK == (ENC_CTRL_HIRQ_MASK & base->CTRL))
    {
        ret32 |= kENC_HOMETransitionFlag;
    }
    if (ENC_CTRL_XIRQ_MASK == (ENC_CTRL_XIRQ_MASK & base->CTRL))
    {
        ret32 |= kENC_INDEXPulseFlag;
    }
    if (ENC_CTRL_DIRQ_MASK == (ENC_CTRL_DIRQ_MASK & base->CTRL))
    {
        ret32 |= kENC_WatchdogTimeoutFlag;
    }
    if (ENC_CTRL_CMPIRQ_MASK == (ENC_CTRL_CMPIRQ_MASK & base->CTRL))
    {
        ret32 |= kENC_PositionCompareFlag;
    }

    /* ENC_CTRL2. */
    if (ENC_CTRL2_SABIRQ_MASK == (ENC_CTRL2_SABIRQ_MASK & base->CTRL2))
    {
        ret32 |= kENC_SimultBothPhaseChangeFlag;
    }
    if (ENC_CTRL2_ROIRQ_MASK == (ENC_CTRL2_ROIRQ_MASK & base->CTRL2))
    {
        ret32 |= kENC_PositionRollOverFlag;
    }
    if (ENC_CTRL2_RUIRQ_MASK == (ENC_CTRL2_RUIRQ_MASK & base->CTRL2))
    {
        ret32 |= kENC_PositionRollUnderFlag;
    }
    if (ENC_CTRL2_DIR_MASK == (ENC_CTRL2_DIR_MASK & base->CTRL2))
    {
        ret32 |= kENC_LastCountDirectionFlag;
    }

    return ret32;
}

/*!
 * brief Clear the status flags.
 *
 * param base ENC peripheral base address.
 * param mask Mask value of status flags to be cleared. For available mask, see to "_enc_status_flags".
 */
void ENC_ClearStatusFlags(ENC_Type *base, uint32_t mask)
{
    uint32_t tmp16 = 0U;

    /* ENC_CTRL. */
    if (kENC_HOMETransitionFlag == (kENC_HOMETransitionFlag & mask))
    {
        tmp16 |= ENC_CTRL_HIRQ_MASK;
    }
    if (kENC_INDEXPulseFlag == (kENC_INDEXPulseFlag & mask))
    {
        tmp16 |= ENC_CTRL_XIRQ_MASK;
    }
    if (kENC_WatchdogTimeoutFlag == (kENC_WatchdogTimeoutFlag & mask))
    {
        tmp16 |= ENC_CTRL_DIRQ_MASK;
    }
    if (kENC_PositionCompareFlag == (kENC_PositionCompareFlag & mask))
    {
        tmp16 |= ENC_CTRL_CMPIRQ_MASK;
    }
    if (0U != tmp16)
    {
        base->CTRL = (base->CTRL & (uint16_t)(~ENC_CTRL_W1C_FLAGS)) | tmp16;
    }

    /* ENC_CTRL2. */
    tmp16 = 0U;
    if (kENC_SimultBothPhaseChangeFlag == (kENC_SimultBothPhaseChangeFlag & mask))
    {
        tmp16 |= ENC_CTRL2_SABIRQ_MASK;
    }
    if (kENC_PositionRollOverFlag == (kENC_PositionRollOverFlag & mask))
    {
        tmp16 |= ENC_CTRL2_ROIRQ_MASK;
    }
    if (kENC_PositionRollUnderFlag == (kENC_PositionRollUnderFlag & mask))
    {
        tmp16 |= ENC_CTRL2_RUIRQ_MASK;
    }
    if (0U != tmp16)
    {
        base->CTRL2 = (base->CTRL2 & (uint16_t)(~ENC_CTRL2_W1C_FLAGS)) | tmp16;
    }
}

/*!
 * brief Enable the interrupts.
 *
 * param base ENC peripheral base address.
 * param mask Mask value of interrupts to be enabled. For available mask, see to "_enc_interrupt_enable".
 */
void ENC_EnableInterrupts(ENC_Type *base, uint32_t mask)
{
    uint32_t tmp16 = 0U;

    /* ENC_CTRL. */
    if (kENC_HOMETransitionInterruptEnable == (kENC_HOMETransitionInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL_HIE_MASK;
    }
    if (kENC_INDEXPulseInterruptEnable == (kENC_INDEXPulseInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL_XIE_MASK;
    }
    if (kENC_WatchdogTimeoutInterruptEnable == (kENC_WatchdogTimeoutInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL_DIE_MASK;
    }
    if (kENC_PositionCompareInerruptEnable == (kENC_PositionCompareInerruptEnable & mask))
    {
        tmp16 |= ENC_CTRL_CMPIE_MASK;
    }
    if (tmp16 != 0U)
    {
        base->CTRL = (base->CTRL & (uint16_t)(~ENC_CTRL_W1C_FLAGS)) | tmp16;
    }
    /* ENC_CTRL2. */
    tmp16 = 0U;
    if (kENC_SimultBothPhaseChangeInterruptEnable == (kENC_SimultBothPhaseChangeInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL2_SABIE_MASK;
    }
    if (kENC_PositionRollOverInterruptEnable == (kENC_PositionRollOverInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL2_ROIE_MASK;
    }
    if (kENC_PositionRollUnderInterruptEnable == (kENC_PositionRollUnderInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL2_RUIE_MASK;
    }
    if (tmp16 != 0U)
    {
        base->CTRL2 = (base->CTRL2 & (uint16_t)(~ENC_CTRL2_W1C_FLAGS)) | tmp16;
    }
}

/*!
 * brief Disable the interrupts.
 *
 * param base ENC peripheral base address.
 * param mask Mask value of interrupts to be disabled. For available mask, see to "_enc_interrupt_enable".
 */
void ENC_DisableInterrupts(ENC_Type *base, uint32_t mask)
{
    uint16_t tmp16 = 0U;

    /* ENC_CTRL. */
    if (kENC_HOMETransitionInterruptEnable == (kENC_HOMETransitionInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL_HIE_MASK;
    }
    if (kENC_INDEXPulseInterruptEnable == (kENC_INDEXPulseInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL_XIE_MASK;
    }
    if (kENC_WatchdogTimeoutInterruptEnable == (kENC_WatchdogTimeoutInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL_DIE_MASK;
    }
    if (kENC_PositionCompareInerruptEnable == (kENC_PositionCompareInerruptEnable & mask))
    {
        tmp16 |= ENC_CTRL_CMPIE_MASK;
    }
    if (0U != tmp16)
    {
        base->CTRL = (uint16_t)(base->CTRL & (uint16_t)(~ENC_CTRL_W1C_FLAGS)) & (uint16_t)(~tmp16);
    }
    /* ENC_CTRL2. */
    tmp16 = 0U;
    if (kENC_SimultBothPhaseChangeInterruptEnable == (kENC_SimultBothPhaseChangeInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL2_SABIE_MASK;
    }
    if (kENC_PositionRollOverInterruptEnable == (kENC_PositionRollOverInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL2_ROIE_MASK;
    }
    if (kENC_PositionRollUnderInterruptEnable == (kENC_PositionRollUnderInterruptEnable & mask))
    {
        tmp16 |= ENC_CTRL2_RUIE_MASK;
    }
    if (tmp16 != 0U)
    {
        base->CTRL2 = (uint16_t)(base->CTRL2 & (uint16_t)(~ENC_CTRL2_W1C_FLAGS)) & (uint16_t)(~tmp16);
    }
}

/*!
 * brief  Get the enabled interrupts' flags.
 *
 * param  base ENC peripheral base address.
 *
 * return      Mask value of enabled interrupts.
 */
uint32_t ENC_GetEnabledInterrupts(ENC_Type *base)
{
    uint32_t ret32 = 0U;

    /* ENC_CTRL. */
    if (ENC_CTRL_HIE_MASK == (ENC_CTRL_HIE_MASK & base->CTRL))
    {
        ret32 |= kENC_HOMETransitionInterruptEnable;
    }
    if (ENC_CTRL_XIE_MASK == (ENC_CTRL_XIE_MASK & base->CTRL))
    {
        ret32 |= kENC_INDEXPulseInterruptEnable;
    }
    if (ENC_CTRL_DIE_MASK == (ENC_CTRL_DIE_MASK & base->CTRL))
    {
        ret32 |= kENC_WatchdogTimeoutInterruptEnable;
    }
    if (ENC_CTRL_CMPIE_MASK == (ENC_CTRL_CMPIE_MASK & base->CTRL))
    {
        ret32 |= kENC_PositionCompareInerruptEnable;
    }
    /* ENC_CTRL2. */
    if (ENC_CTRL2_SABIE_MASK == (ENC_CTRL2_SABIE_MASK & base->CTRL2))
    {
        ret32 |= kENC_SimultBothPhaseChangeInterruptEnable;
    }
    if (ENC_CTRL2_ROIE_MASK == (ENC_CTRL2_ROIE_MASK & base->CTRL2))
    {
        ret32 |= kENC_PositionRollOverInterruptEnable;
    }
    if (ENC_CTRL2_RUIE_MASK == (ENC_CTRL2_RUIE_MASK & base->CTRL2))
    {
        ret32 |= kENC_PositionRollUnderInterruptEnable;
    }
    return ret32;
}

/*!
 * brief Set initial position value for ENC module.
 *
 * param base ENC peripheral base address
 * param value Positive initial value
 */
void ENC_SetInitialPositionValue(ENC_Type *base, uint32_t value)
{
    base->UINIT = (uint16_t)(value >> 16U); /* Set upper 16 bits. */
    base->LINIT = (uint16_t)(value);        /* Set lower 16 bits. */
}

/*!
 * brief  Get the current position counter's value.
 *
 * param  base ENC peripheral base address.
 *
 * return     Current position counter's value.
 */
uint32_t ENC_GetPositionValue(ENC_Type *base)
{
    uint32_t ret32;

    ret32 = base->UPOS; /* Get upper 16 bits and make a snapshot. */
    ret32 <<= 16U;
    ret32 |= base->LPOSH; /* Get lower 16 bits from hold register. */

    return ret32;
}

/*!
 * brief  Get the hold position counter's value.
 *
 * When any of the counter registers is read, the contents of each counter register is written to the corresponding hold
 * register. Taking a snapshot of the counters' values provides a consistent view of a system position and a velocity to
 * be attained.
 *
 * param  base ENC peripheral base address.
 *
 * return      Hold position counter's value.
 */
uint32_t ENC_GetHoldPositionValue(ENC_Type *base)
{
    uint32_t ret32;

    ret32 = base->UPOSH; /* Get upper 16 bits and make a snapshot. */
    ret32 <<= 16U;
    ret32 |= base->LPOSH; /* Get lower 16 bits from hold register. */

    return ret32;
}
