/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_pwm.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.pwm"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the instance from the base address
 *
 * @param base PWM peripheral base address
 *
 * @return The PWM module instance
 */
static uint32_t PWM_GetInstance(PWM_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to PWM bases for each instance. */
static PWM_Type *const s_pwmBases[] = PWM_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to PWM clocks for each PWM submodule. */
static const clock_ip_name_t s_pwmClocks[][FSL_FEATURE_PWM_SUBMODULE_COUNT] = PWM_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t PWM_GetInstance(PWM_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_pwmBases); instance++)
    {
        if (s_pwmBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_pwmBases));

    return instance;
}

/*!
 * brief Ungates the PWM submodule clock and configures the peripheral for basic operation.
 *
 * note This API should be called at the beginning of the application using the PWM driver.
 *
 * param base      PWM peripheral base address
 * param subModule PWM submodule to configure
 * param config    Pointer to user's PWM config structure.
 *
 * return kStatus_Success means success; else failed.
 */
status_t PWM_Init(PWM_Type *base, pwm_submodule_t subModule, const pwm_config_t *config)
{
    assert(config);

    uint16_t reg;

    /* Source clock for submodule 0 cannot be itself */
    if ((config->clockSource == kPWM_Submodule0Clock) && (subModule == kPWM_Module_0))
    {
        return kStatus_Fail;
    }

    /* Reload source select clock for submodule 0 cannot be master reload */
    if ((config->reloadSelect == kPWM_MasterReload) && (subModule == kPWM_Module_0))
    {
        return kStatus_Fail;
    }

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate the PWM submodule clock*/
    CLOCK_EnableClock(s_pwmClocks[PWM_GetInstance(base)][subModule]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Clear the fault status flags */
    base->FSTS |= PWM_FSTS_FFLAG_MASK;

    reg = base->SM[subModule].CTRL2;

    /* Setup the submodule clock-source, control source of the INIT signal,
     * source of the force output signal, operation in debug & wait modes and reload source select
     */
    reg &= ~(PWM_CTRL2_CLK_SEL_MASK | PWM_CTRL2_FORCE_SEL_MASK | PWM_CTRL2_INIT_SEL_MASK | PWM_CTRL2_INDEP_MASK |
             PWM_CTRL2_WAITEN_MASK | PWM_CTRL2_DBGEN_MASK | PWM_CTRL2_RELOAD_SEL_MASK);
    reg |= (PWM_CTRL2_CLK_SEL(config->clockSource) | PWM_CTRL2_FORCE_SEL(config->forceTrigger) |
            PWM_CTRL2_INIT_SEL(config->initializationControl) | PWM_CTRL2_DBGEN(config->enableDebugMode) |
            PWM_CTRL2_WAITEN(config->enableWait) | PWM_CTRL2_RELOAD_SEL(config->reloadSelect));

    /* Setup PWM A & B to be independent or a complementary-pair */
    switch (config->pairOperation)
    {
        case kPWM_Independent:
            reg |= PWM_CTRL2_INDEP_MASK;
            break;
        case kPWM_ComplementaryPwmA:
            base->MCTRL &= ~(1U << (PWM_MCTRL_IPOL_SHIFT + subModule));
            break;
        case kPWM_ComplementaryPwmB:
            base->MCTRL |= (1U << (PWM_MCTRL_IPOL_SHIFT + subModule));
            break;
        default:
            break;
    }
    base->SM[subModule].CTRL2 = reg;

    reg = base->SM[subModule].CTRL;

    /* Setup the clock prescale, load mode and frequency */
    reg &= ~(PWM_CTRL_PRSC_MASK | PWM_CTRL_LDFQ_MASK | PWM_CTRL_LDMOD_MASK);
    reg |= (PWM_CTRL_PRSC(config->prescale) | PWM_CTRL_LDFQ(config->reloadFrequency));

    /* Setup register reload logic */
    switch (config->reloadLogic)
    {
        case kPWM_ReloadImmediate:
            reg |= PWM_CTRL_LDMOD_MASK;
            break;
        case kPWM_ReloadPwmHalfCycle:
            reg |= PWM_CTRL_HALF_MASK;
            reg &= ~PWM_CTRL_FULL_MASK;
            break;
        case kPWM_ReloadPwmFullCycle:
            reg &= ~PWM_CTRL_HALF_MASK;
            reg |= PWM_CTRL_FULL_MASK;
            break;
        case kPWM_ReloadPwmHalfAndFullCycle:
            reg |= PWM_CTRL_HALF_MASK;
            reg |= PWM_CTRL_FULL_MASK;
            break;
        default:
            break;
    }
    base->SM[subModule].CTRL = reg;

    /* Setup the fault filter */
    if (base->FFILT & PWM_FFILT_FILT_PER_MASK)
    {
        /* When changing values for fault period from a non-zero value, first write a value of 0
         * to clear the filter
         */
        base->FFILT &= ~(PWM_FFILT_FILT_PER_MASK);
    }

    base->FFILT = (PWM_FFILT_FILT_CNT(config->faultFilterCount) | PWM_FFILT_FILT_PER(config->faultFilterPeriod));

    /* Issue a Force trigger event when configured to trigger locally */
    if (config->forceTrigger == kPWM_Force_Local)
    {
        base->SM[subModule].CTRL2 |= PWM_CTRL2_FORCE(1U);
    }

    return kStatus_Success;
}

/*!
 * brief Gate the PWM submodule clock
 *
 * param base      PWM peripheral base address
 * param subModule PWM submodule to deinitialize
 */
void PWM_Deinit(PWM_Type *base, pwm_submodule_t subModule)
{
    /* Stop the submodule */
    base->MCTRL &= ~(1U << (PWM_MCTRL_RUN_SHIFT + subModule));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Gate the PWM submodule clock*/
    CLOCK_DisableClock(s_pwmClocks[PWM_GetInstance(base)][subModule]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief  Fill in the PWM config struct with the default settings
 *
 * The default values are:
 * code
 *   config->enableDebugMode = false;
 *   config->enableWait = false;
 *   config->reloadSelect = kPWM_LocalReload;
 *   config->faultFilterCount = 0;
 *   config->faultFilterPeriod = 0;
 *   config->clockSource = kPWM_BusClock;
 *   config->prescale = kPWM_Prescale_Divide_1;
 *   config->initializationControl = kPWM_Initialize_LocalSync;
 *   config->forceTrigger = kPWM_Force_Local;
 *   config->reloadFrequency = kPWM_LoadEveryOportunity;
 *   config->reloadLogic = kPWM_ReloadImmediate;
 *   config->pairOperation = kPWM_Independent;
 * endcode
 * param config Pointer to user's PWM config structure.
 */
void PWM_GetDefaultConfig(pwm_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    /* PWM is paused in debug mode */
    config->enableDebugMode = false;
    /* PWM is paused in wait mode */
    config->enableWait = false;
    /* PWM module uses the local reload signal to reload registers */
    config->reloadSelect = kPWM_LocalReload;
    /* Fault filter count is set to 0 */
    config->faultFilterCount = 0;
    /* Fault filter period is set to 0 which disables the fault filter */
    config->faultFilterPeriod = 0;
    /* Use the IP Bus clock as source clock for the PWM submodule */
    config->clockSource = kPWM_BusClock;
    /* Clock source prescale is set to divide by 1*/
    config->prescale = kPWM_Prescale_Divide_1;
    /* Local sync causes initialization */
    config->initializationControl = kPWM_Initialize_LocalSync;
    /* The local force signal, CTRL2[FORCE], from the submodule is used to force updates */
    config->forceTrigger = kPWM_Force_Local;
    /* PWM reload frequency, reload opportunity is PWM half cycle or full cycle.
     * This field is not used in Immediate reload mode
     */
    config->reloadFrequency = kPWM_LoadEveryOportunity;
    /* Buffered-registers get loaded with new values as soon as LDOK bit is set */
    config->reloadLogic = kPWM_ReloadImmediate;
    /* PWM A & PWM B operate as 2 independent channels */
    config->pairOperation = kPWM_Independent;
}

/*!
 * brief Sets up the PWM signals for a PWM submodule.
 *
 * The function initializes the submodule according to the parameters passed in by the user. The function
 * also sets up the value compare registers to match the PWM signal requirements.
 * If the dead time insertion logic is enabled, the pulse period is reduced by the
 * dead time period specified by the user.
 *
 * param base        PWM peripheral base address
 * param subModule   PWM submodule to configure
 * param chnlParams  Array of PWM channel parameters to configure the channel(s)
 * param numOfChnls  Number of channels to configure, this should be the size of the array passed in.
 *                    Array size should not be more than 2 as each submodule has 2 pins to output PWM
 * param mode        PWM operation mode, options available in enumeration ::pwm_mode_t
 * param pwmFreq_Hz  PWM signal frequency in Hz
 * param srcClock_Hz PWM main counter clock in Hz.
 *
 * return Returns kStatusFail if there was error setting up the signal; kStatusSuccess otherwise
 */
status_t PWM_SetupPwm(PWM_Type *base,
                      pwm_submodule_t subModule,
                      const pwm_signal_param_t *chnlParams,
                      uint8_t numOfChnls,
                      pwm_mode_t mode,
                      uint32_t pwmFreq_Hz,
                      uint32_t srcClock_Hz)
{
    assert(chnlParams);
    assert(pwmFreq_Hz);
    assert(numOfChnls);
    assert(srcClock_Hz);

    uint32_t pwmClock;
    uint16_t pulseCnt = 0, pwmHighPulse = 0;
    int16_t modulo = 0;
    uint8_t i, polarityShift = 0, outputEnableShift = 0;

    if (numOfChnls > 2)
    {
        /* Each submodule has 2 signals; PWM A & PWM B */
        return kStatus_Fail;
    }

    /* Divide the clock by the prescale value */
    pwmClock = (srcClock_Hz / (1U << ((base->SM[subModule].CTRL & PWM_CTRL_PRSC_MASK) >> PWM_CTRL_PRSC_SHIFT)));
    pulseCnt = (pwmClock / pwmFreq_Hz);

    /* Setup each PWM channel */
    for (i = 0; i < numOfChnls; i++)
    {
        /* Calculate pulse width */
        pwmHighPulse = (pulseCnt * chnlParams->dutyCyclePercent) / 100;

        /* Setup the different match registers to generate the PWM signal */
        switch (mode)
        {
            case kPWM_SignedCenterAligned:
                /* Setup the PWM period for a signed center aligned signal */
                modulo = pulseCnt >> 1;
                /* Indicates the start of the PWM period */
                base->SM[subModule].INIT = (-modulo);
                /* Indicates the center value */
                base->SM[subModule].VAL0 = 0;
                /* Indicates the end of the PWM period */
                base->SM[subModule].VAL1 = modulo;

                /* Setup the PWM dutycycle */
                if (chnlParams->pwmChannel == kPWM_PwmA)
                {
                    base->SM[subModule].VAL2 = (-(pwmHighPulse / 2));
                    base->SM[subModule].VAL3 = (pwmHighPulse / 2);
                }
                else
                {
                    base->SM[subModule].VAL4 = (-(pwmHighPulse / 2));
                    base->SM[subModule].VAL5 = (pwmHighPulse / 2);
                }
                break;
            case kPWM_CenterAligned:
                /* Setup the PWM period for an unsigned center aligned signal */
                /* Indicates the start of the PWM period */
                base->SM[subModule].INIT = 0;
                /* Indicates the center value */
                base->SM[subModule].VAL0 = (pulseCnt / 2);
                /* Indicates the end of the PWM period */
                base->SM[subModule].VAL1 = pulseCnt;

                /* Setup the PWM dutycycle */
                if (chnlParams->pwmChannel == kPWM_PwmA)
                {
                    base->SM[subModule].VAL2 = ((pulseCnt - pwmHighPulse) / 2);
                    base->SM[subModule].VAL3 = ((pulseCnt + pwmHighPulse) / 2);
                }
                else
                {
                    base->SM[subModule].VAL4 = ((pulseCnt - pwmHighPulse) / 2);
                    base->SM[subModule].VAL5 = ((pulseCnt + pwmHighPulse) / 2);
                }
                break;
            case kPWM_SignedEdgeAligned:
                /* Setup the PWM period for a signed edge aligned signal */
                modulo = pulseCnt >> 1;
                /* Indicates the start of the PWM period */
                base->SM[subModule].INIT = (-modulo);
                /* Indicates the center value */
                base->SM[subModule].VAL0 = 0;
                /* Indicates the end of the PWM period */
                base->SM[subModule].VAL1 = modulo;

                /* Setup the PWM dutycycle */
                if (chnlParams->pwmChannel == kPWM_PwmA)
                {
                    base->SM[subModule].VAL2 = (-modulo);
                    base->SM[subModule].VAL3 = (-modulo + pwmHighPulse);
                }
                else
                {
                    base->SM[subModule].VAL4 = (-modulo);
                    base->SM[subModule].VAL5 = (-modulo + pwmHighPulse);
                }
                break;
            case kPWM_EdgeAligned:
                /* Setup the PWM period for a unsigned edge aligned signal */
                /* Indicates the start of the PWM period */
                base->SM[subModule].INIT = 0;
                /* Indicates the center value */
                base->SM[subModule].VAL0 = (pulseCnt / 2);
                /* Indicates the end of the PWM period */
                base->SM[subModule].VAL1 = pulseCnt;

                /* Setup the PWM dutycycle */
                if (chnlParams->pwmChannel == kPWM_PwmA)
                {
                    base->SM[subModule].VAL2 = 0;
                    base->SM[subModule].VAL3 = pwmHighPulse;
                }
                else
                {
                    base->SM[subModule].VAL4 = 0;
                    base->SM[subModule].VAL5 = pwmHighPulse;
                }
                break;
            default:
                break;
        }
        /* Setup register shift values based on the channel being configured.
         * Also setup the deadtime value
         */
        if (chnlParams->pwmChannel == kPWM_PwmA)
        {
            polarityShift              = PWM_OCTRL_POLA_SHIFT;
            outputEnableShift          = PWM_OUTEN_PWMA_EN_SHIFT;
            base->SM[subModule].DTCNT0 = PWM_DTCNT0_DTCNT0(chnlParams->deadtimeValue);
        }
        else
        {
            polarityShift              = PWM_OCTRL_POLB_SHIFT;
            outputEnableShift          = PWM_OUTEN_PWMB_EN_SHIFT;
            base->SM[subModule].DTCNT1 = PWM_DTCNT1_DTCNT1(chnlParams->deadtimeValue);
        }

        /* Setup signal active level */
        if (chnlParams->level == kPWM_HighTrue)
        {
            base->SM[subModule].OCTRL &= ~(1U << polarityShift);
        }
        else
        {
            base->SM[subModule].OCTRL |= (1U << polarityShift);
        }
        /* Enable PWM output */
        base->OUTEN |= (1U << (outputEnableShift + subModule));

        /* Get the next channel parameters */
        chnlParams++;
    }

    return kStatus_Success;
}

/*!
 * brief Updates the PWM signal's dutycycle.
 *
 * The function updates the PWM dutycyle to the new value that is passed in.
 * If the dead time insertion logic is enabled then the pulse period is reduced by the
 * dead time period specified by the user.
 *
 * param base              PWM peripheral base address
 * param subModule         PWM submodule to configure
 * param pwmSignal         Signal (PWM A or PWM B) to update
 * param currPwmMode       The current PWM mode set during PWM setup
 * param dutyCyclePercent  New PWM pulse width, value should be between 0 to 100
 *                          0=inactive signal(0% duty cycle)...
 *                          100=active signal (100% duty cycle)
 */
void PWM_UpdatePwmDutycycle(PWM_Type *base,
                            pwm_submodule_t subModule,
                            pwm_channels_t pwmSignal,
                            pwm_mode_t currPwmMode,
                            uint8_t dutyCyclePercent)
{
    assert(dutyCyclePercent <= 100);
    assert(pwmSignal < 2);
    uint16_t pulseCnt = 0, pwmHighPulse = 0;
    int16_t modulo = 0;

    switch (currPwmMode)
    {
        case kPWM_SignedCenterAligned:
            modulo   = base->SM[subModule].VAL1;
            pulseCnt = modulo * 2;
            /* Calculate pulse width */
            pwmHighPulse = (pulseCnt * dutyCyclePercent) / 100;

            /* Setup the PWM dutycycle */
            if (pwmSignal == kPWM_PwmA)
            {
                base->SM[subModule].VAL2 = (-(pwmHighPulse / 2));
                base->SM[subModule].VAL3 = (pwmHighPulse / 2);
            }
            else
            {
                base->SM[subModule].VAL4 = (-(pwmHighPulse / 2));
                base->SM[subModule].VAL5 = (pwmHighPulse / 2);
            }
            break;
        case kPWM_CenterAligned:
            pulseCnt = base->SM[subModule].VAL1;
            /* Calculate pulse width */
            pwmHighPulse = (pulseCnt * dutyCyclePercent) / 100;

            /* Setup the PWM dutycycle */
            if (pwmSignal == kPWM_PwmA)
            {
                base->SM[subModule].VAL2 = ((pulseCnt - pwmHighPulse) / 2);
                base->SM[subModule].VAL3 = ((pulseCnt + pwmHighPulse) / 2);
            }
            else
            {
                base->SM[subModule].VAL4 = ((pulseCnt - pwmHighPulse) / 2);
                base->SM[subModule].VAL5 = ((pulseCnt + pwmHighPulse) / 2);
            }
            break;
        case kPWM_SignedEdgeAligned:
            modulo   = base->SM[subModule].VAL1;
            pulseCnt = modulo * 2;
            /* Calculate pulse width */
            pwmHighPulse = (pulseCnt * dutyCyclePercent) / 100;

            /* Setup the PWM dutycycle */
            if (pwmSignal == kPWM_PwmA)
            {
                base->SM[subModule].VAL2 = (-modulo);
                base->SM[subModule].VAL3 = (-modulo + pwmHighPulse);
            }
            else
            {
                base->SM[subModule].VAL4 = (-modulo);
                base->SM[subModule].VAL5 = (-modulo + pwmHighPulse);
            }
            break;
        case kPWM_EdgeAligned:
            pulseCnt = base->SM[subModule].VAL1;
            /* Calculate pulse width */
            pwmHighPulse = (pulseCnt * dutyCyclePercent) / 100;

            /* Setup the PWM dutycycle */
            if (pwmSignal == kPWM_PwmA)
            {
                base->SM[subModule].VAL2 = 0;
                base->SM[subModule].VAL3 = pwmHighPulse;
            }
            else
            {
                base->SM[subModule].VAL4 = 0;
                base->SM[subModule].VAL5 = pwmHighPulse;
            }
            break;
        default:
            break;
    }
}

/*!
 * brief Sets up the PWM input capture
 *
 * Each PWM submodule has 3 pins that can be configured for use as input capture pins. This function
 * sets up the capture parameters for each pin and enables the pin for input capture operation.
 *
 * param base               PWM peripheral base address
 * param subModule          PWM submodule to configure
 * param pwmChannel         Channel in the submodule to setup
 * param inputCaptureParams Parameters passed in to set up the input pin
 */
void PWM_SetupInputCapture(PWM_Type *base,
                           pwm_submodule_t subModule,
                           pwm_channels_t pwmChannel,
                           const pwm_input_capture_param_t *inputCaptureParams)
{
    uint32_t reg = 0;
    switch (pwmChannel)
    {
        case kPWM_PwmA:
            /* Setup the capture paramters for PWM A pin */
            reg = (PWM_CAPTCTRLA_INP_SELA(inputCaptureParams->captureInputSel) |
                   PWM_CAPTCTRLA_EDGA0(inputCaptureParams->edge0) | PWM_CAPTCTRLA_EDGA1(inputCaptureParams->edge1) |
                   PWM_CAPTCTRLA_ONESHOTA(inputCaptureParams->enableOneShotCapture) |
                   PWM_CAPTCTRLA_CFAWM(inputCaptureParams->fifoWatermark));
            /* Enable the edge counter if using the output edge counter */
            if (inputCaptureParams->captureInputSel)
            {
                reg |= PWM_CAPTCTRLA_EDGCNTA_EN_MASK;
            }
            /* Enable input capture operation */
            reg |= PWM_CAPTCTRLA_ARMA_MASK;

            base->SM[subModule].CAPTCTRLA = reg;

            /* Setup the compare value when using the edge counter as source */
            base->SM[subModule].CAPTCOMPA = PWM_CAPTCOMPA_EDGCMPA(inputCaptureParams->edgeCompareValue);
            /* Setup PWM A pin for input capture */
            base->OUTEN &= ~(1U << (PWM_OUTEN_PWMA_EN_SHIFT + subModule));

            break;
        case kPWM_PwmB:
            /* Setup the capture paramters for PWM B pin */
            reg = (PWM_CAPTCTRLB_INP_SELB(inputCaptureParams->captureInputSel) |
                   PWM_CAPTCTRLB_EDGB0(inputCaptureParams->edge0) | PWM_CAPTCTRLB_EDGB1(inputCaptureParams->edge1) |
                   PWM_CAPTCTRLB_ONESHOTB(inputCaptureParams->enableOneShotCapture) |
                   PWM_CAPTCTRLB_CFBWM(inputCaptureParams->fifoWatermark));
            /* Enable the edge counter if using the output edge counter */
            if (inputCaptureParams->captureInputSel)
            {
                reg |= PWM_CAPTCTRLB_EDGCNTB_EN_MASK;
            }
            /* Enable input capture operation */
            reg |= PWM_CAPTCTRLB_ARMB_MASK;

            base->SM[subModule].CAPTCTRLB = reg;

            /* Setup the compare value when using the edge counter as source */
            base->SM[subModule].CAPTCOMPB = PWM_CAPTCOMPB_EDGCMPB(inputCaptureParams->edgeCompareValue);
            /* Setup PWM B pin for input capture */
            base->OUTEN &= ~(1U << (PWM_OUTEN_PWMB_EN_SHIFT + subModule));
            break;
        case kPWM_PwmX:
            reg = (PWM_CAPTCTRLX_INP_SELX(inputCaptureParams->captureInputSel) |
                   PWM_CAPTCTRLX_EDGX0(inputCaptureParams->edge0) | PWM_CAPTCTRLX_EDGX1(inputCaptureParams->edge1) |
                   PWM_CAPTCTRLX_ONESHOTX(inputCaptureParams->enableOneShotCapture) |
                   PWM_CAPTCTRLX_CFXWM(inputCaptureParams->fifoWatermark));
            /* Enable the edge counter if using the output edge counter */
            if (inputCaptureParams->captureInputSel)
            {
                reg |= PWM_CAPTCTRLX_EDGCNTX_EN_MASK;
            }
            /* Enable input capture operation */
            reg |= PWM_CAPTCTRLX_ARMX_MASK;

            base->SM[subModule].CAPTCTRLX = reg;

            /* Setup the compare value when using the edge counter as source */
            base->SM[subModule].CAPTCOMPX = PWM_CAPTCOMPX_EDGCMPX(inputCaptureParams->edgeCompareValue);
            /* Setup PWM X pin for input capture */
            base->OUTEN &= ~(1U << (PWM_OUTEN_PWMX_EN_SHIFT + subModule));
            break;
        default:
            break;
    }
}

/*!
 * brief Sets up the PWM fault protection.
 *
 * PWM has 4 fault inputs.
 *
 * param base        PWM peripheral base address
 * param faultNum    PWM fault to configure.
 * param faultParams Pointer to the PWM fault config structure
 */
void PWM_SetupFaults(PWM_Type *base, pwm_fault_input_t faultNum, const pwm_fault_param_t *faultParams)
{
    assert(faultParams);
    uint16_t reg;

    reg = base->FCTRL;
    /* Set the faults level-settting */
    if (faultParams->faultLevel)
    {
        reg |= (1U << (PWM_FCTRL_FLVL_SHIFT + faultNum));
    }
    else
    {
        reg &= ~(1U << (PWM_FCTRL_FLVL_SHIFT + faultNum));
    }
    /* Set the fault clearing mode */
    if (faultParams->faultClearingMode)
    {
        /* Use manual fault clearing */
        reg &= ~(1U << (PWM_FCTRL_FAUTO_SHIFT + faultNum));
        if (faultParams->faultClearingMode == kPWM_ManualSafety)
        {
            /* Use manual fault clearing with safety mode enabled */
            reg |= (1U << (PWM_FCTRL_FSAFE_SHIFT + faultNum));
        }
        else
        {
            /* Use manual fault clearing with safety mode disabled */
            reg &= ~(1U << (PWM_FCTRL_FSAFE_SHIFT + faultNum));
        }
    }
    else
    {
        /* Use automatic fault clearing */
        reg |= (1U << (PWM_FCTRL_FAUTO_SHIFT + faultNum));
    }
    base->FCTRL = reg;

    /* Set the combinational path option */
    if (faultParams->enableCombinationalPath)
    {
        /* Combinational path from the fault input to the PWM output is available */
        base->FCTRL2 &= ~(1U << faultNum);
    }
    else
    {
        /* No combinational path available, only fault filter & latch signal can disable PWM output */
        base->FCTRL2 |= (1U << faultNum);
    }

    /* Initially clear both recovery modes */
    reg = base->FSTS;
    reg &= ~((1U << (PWM_FSTS_FFULL_SHIFT + faultNum)) | (1U << (PWM_FSTS_FHALF_SHIFT + faultNum)));
    /* Setup fault recovery */
    switch (faultParams->recoverMode)
    {
        case kPWM_NoRecovery:
            break;
        case kPWM_RecoverHalfCycle:
            reg |= (1U << (PWM_FSTS_FHALF_SHIFT + faultNum));
            break;
        case kPWM_RecoverFullCycle:
            reg |= (1U << (PWM_FSTS_FFULL_SHIFT + faultNum));
            break;
        case kPWM_RecoverHalfAndFullCycle:
            reg |= (1U << (PWM_FSTS_FHALF_SHIFT + faultNum));
            reg |= (1U << (PWM_FSTS_FFULL_SHIFT + faultNum));
            break;
        default:
            break;
    }
    base->FSTS = reg;
}

/*!
 * brief Selects the signal to output on a PWM pin when a FORCE_OUT signal is asserted.
 *
 * The user specifies which channel to configure by supplying the submodule number and whether
 * to modify PWM A or PWM B within that submodule.
 *
 * param base       PWM peripheral base address
 * param subModule  PWM submodule to configure
 * param pwmChannel Channel to configure
 * param mode       Signal to output when a FORCE_OUT is triggered
 */
void PWM_SetupForceSignal(PWM_Type *base, pwm_submodule_t subModule, pwm_channels_t pwmChannel, pwm_force_signal_t mode)

{
    uint16_t shift;
    uint16_t reg;

    /* DTSRCSEL register has 4 bits per submodule; 2 bits for PWM A and 2 bits for PWM B */
    shift = subModule * 4 + pwmChannel * 2;

    /* Setup the signal to be passed upon occurrence of a FORCE_OUT signal */
    reg = base->DTSRCSEL;
    reg &= ~(0x3U << shift);
    reg |= (uint16_t)((uint16_t)mode << shift);
    base->DTSRCSEL = reg;
}

/*!
 * brief Enables the selected PWM interrupts
 *
 * param base      PWM peripheral base address
 * param subModule PWM submodule to configure
 * param mask      The interrupts to enable. This is a logical OR of members of the
 *                  enumeration ::pwm_interrupt_enable_t
 */
void PWM_EnableInterrupts(PWM_Type *base, pwm_submodule_t subModule, uint32_t mask)
{
    /* Upper 16 bits are for related to the submodule */
    base->SM[subModule].INTEN |= (mask & 0xFFFFU);
    /* Fault related interrupts */
    base->FCTRL |= ((mask >> 16U) & PWM_FCTRL_FIE_MASK);
}

/*!
 * brief Disables the selected PWM interrupts
 *
 * param base      PWM peripheral base address
 * param subModule PWM submodule to configure
 * param mask      The interrupts to enable. This is a logical OR of members of the
 *                  enumeration ::pwm_interrupt_enable_t
 */
void PWM_DisableInterrupts(PWM_Type *base, pwm_submodule_t subModule, uint32_t mask)
{
    base->SM[subModule].INTEN &= ~(mask & 0xFFFF);
    base->FCTRL &= ~((mask >> 16U) & PWM_FCTRL_FIE_MASK);
}

/*!
 * brief Gets the enabled PWM interrupts
 *
 * param base      PWM peripheral base address
 * param subModule PWM submodule to configure
 *
 * return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::pwm_interrupt_enable_t
 */
uint32_t PWM_GetEnabledInterrupts(PWM_Type *base, pwm_submodule_t subModule)
{
    uint32_t enabledInterrupts;

    enabledInterrupts = base->SM[subModule].INTEN;
    enabledInterrupts |= ((uint32_t)(base->FCTRL & PWM_FCTRL_FIE_MASK) << 16U);
    return enabledInterrupts;
}

/*!
 * brief Gets the PWM status flags
 *
 * param base      PWM peripheral base address
 * param subModule PWM submodule to configure
 *
 * return The status flags. This is the logical OR of members of the
 *         enumeration ::pwm_status_flags_t
 */
uint32_t PWM_GetStatusFlags(PWM_Type *base, pwm_submodule_t subModule)
{
    uint32_t statusFlags;

    statusFlags = base->SM[subModule].STS;
    statusFlags |= ((uint32_t)(base->FSTS & PWM_FSTS_FFLAG_MASK) << 16U);

    return statusFlags;
}

/*!
 * brief Clears the PWM status flags
 *
 * param base      PWM peripheral base address
 * param subModule PWM submodule to configure
 * param mask      The status flags to clear. This is a logical OR of members of the
 *                  enumeration ::pwm_status_flags_t
 */
void PWM_ClearStatusFlags(PWM_Type *base, pwm_submodule_t subModule, uint32_t mask)
{
    uint16_t reg;

    base->SM[subModule].STS = (mask & 0xFFFFU);
    reg                     = base->FSTS;
    /* Clear the fault flags and set only the ones we wish to clear as the fault flags are cleared
     * by writing a login one
     */
    reg &= ~(PWM_FSTS_FFLAG_MASK);
    reg |= ((mask >> 16U) & PWM_FSTS_FFLAG_MASK);
    base->FSTS = reg;
}
