/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_qtmr.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.qtmr"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Gets the instance from the base address to be used to gate or ungate the module clock
 *
 * @param base Quad Timer peripheral base address
 *
 * @return The Quad Timer instance
 */
static uint32_t QTMR_GetInstance(TMR_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Pointers to Quad Timer bases for each instance. */
static TMR_Type *const s_qtmrBases[] = TMR_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to Quad Timer clocks for each instance. */
static const clock_ip_name_t s_qtmrClocks[] = TMR_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t QTMR_GetInstance(TMR_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_qtmrBases); instance++)
    {
        if (s_qtmrBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_qtmrBases));

    return instance;
}

/*!
 * brief Ungates the Quad Timer clock and configures the peripheral for basic operation.
 *
 * note This API should be called at the beginning of the application using the Quad Timer driver.
 *
 * param base     Quad Timer peripheral base address
 * param channel  Quad Timer channel number
 * param config   Pointer to user's Quad Timer config structure
 */
void QTMR_Init(TMR_Type *base, qtmr_channel_selection_t channel, const qtmr_config_t *config)
{
    assert(config);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable the module clock */
    CLOCK_EnableClock(s_qtmrClocks[QTMR_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Setup the counter sources */
    base->CHANNEL[channel].CTRL = (TMR_CTRL_PCS(config->primarySource) | TMR_CTRL_SCS(config->secondarySource));

    /* Setup the master mode operation */
    base->CHANNEL[channel].SCTRL =
        (TMR_SCTRL_EEOF(config->enableExternalForce) | TMR_SCTRL_MSTR(config->enableMasterMode));

    /* Setup debug mode */
    base->CHANNEL[channel].CSCTRL = TMR_CSCTRL_DBG_EN(config->debugMode);

    base->CHANNEL[channel].FILT &= ~(TMR_FILT_FILT_CNT_MASK | TMR_FILT_FILT_PER_MASK);
    /* Setup input filter */
    base->CHANNEL[channel].FILT =
        (TMR_FILT_FILT_CNT(config->faultFilterCount) | TMR_FILT_FILT_PER(config->faultFilterPeriod));
}

/*!
 * brief Stops the counter and gates the Quad Timer clock
 *
 * param base     Quad Timer peripheral base address
 * param channel  Quad Timer channel number
 */
void QTMR_Deinit(TMR_Type *base, qtmr_channel_selection_t channel)
{
    /* Stop the counter */
    base->CHANNEL[channel].CTRL &= ~TMR_CTRL_CM_MASK;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable the module clock */
    CLOCK_DisableClock(s_qtmrClocks[QTMR_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief  Fill in the Quad Timer config struct with the default settings
 *
 * The default values are:
 * code
 *    config->debugMode = kQTMR_RunNormalInDebug;
 *    config->enableExternalForce = false;
 *    config->enableMasterMode = false;
 *    config->faultFilterCount = 0;
 *    config->faultFilterPeriod = 0;
 *    config->primarySource = kQTMR_ClockDivide_2;
 *    config->secondarySource = kQTMR_Counter0InputPin;
 * endcode
 * param config Pointer to user's Quad Timer config structure.
 */
void QTMR_GetDefaultConfig(qtmr_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    /* Halt counter during debug mode */
    config->debugMode = kQTMR_RunNormalInDebug;
    /* Another counter cannot force state of OFLAG signal */
    config->enableExternalForce = false;
    /* Compare function's output from this counter is not broadcast to other counters */
    config->enableMasterMode = false;
    /* Fault filter count is set to 0 */
    config->faultFilterCount = 0;
    /* Fault filter period is set to 0 which disables the fault filter */
    config->faultFilterPeriod = 0;
    /* Primary count source is IP bus clock divide by 2 */
    config->primarySource = kQTMR_ClockDivide_2;
    /* Secondary count source is counter 0 input pin */
    config->secondarySource = kQTMR_Counter0InputPin;
}

/*!
 * brief Sets up Quad timer module for PWM signal output.
 *
 * The function initializes the timer module according to the parameters passed in by the user. The
 * function also sets up the value compare registers to match the PWM signal requirements.
 *
 * param base             Quad Timer peripheral base address
 * param channel          Quad Timer channel number
 * param pwmFreqHz        PWM signal frequency in Hz
 * param dutyCyclePercent PWM pulse width, value should be between 0 to 100
 *                         0=inactive signal(0% duty cycle)...
 *                         100=active signal (100% duty cycle)
 * param outputPolarity   true: invert polarity of the output signal, false: no inversion
 * param srcClock_Hz      Main counter clock in Hz.
 *
 * return Returns an error if there was error setting up the signal.
 */
status_t QTMR_SetupPwm(TMR_Type *base,
                       qtmr_channel_selection_t channel,
                       uint32_t pwmFreqHz,
                       uint8_t dutyCyclePercent,
                       bool outputPolarity,
                       uint32_t srcClock_Hz)
{
    uint32_t periodCount, highCount, lowCount, reg;

    if (dutyCyclePercent > 100)
    {
        /* Invalid dutycycle */
        return kStatus_Fail;
    }

    /* Set OFLAG pin for output mode and force out a low on the pin */
    base->CHANNEL[channel].SCTRL |= (TMR_SCTRL_FORCE_MASK | TMR_SCTRL_OEN_MASK);

    /* Counter values to generate a PWM signal */
    periodCount = (srcClock_Hz / pwmFreqHz);
    highCount   = (periodCount * dutyCyclePercent) / 100;
    lowCount    = periodCount - highCount;

    /* Setup the compare registers for PWM output */
    base->CHANNEL[channel].COMP1 = lowCount;
    base->CHANNEL[channel].COMP2 = highCount;

    /* Setup the pre-load registers for PWM output */
    base->CHANNEL[channel].CMPLD1 = lowCount;
    base->CHANNEL[channel].CMPLD2 = highCount;

    reg = base->CHANNEL[channel].CSCTRL;
    /* Setup the compare load control for COMP1 and COMP2.
     * Load COMP1 when CSCTRL[TCF2] is asserted, load COMP2 when CSCTRL[TCF1] is asserted
     */
    reg &= ~(TMR_CSCTRL_CL1_MASK | TMR_CSCTRL_CL2_MASK);
    reg |= (TMR_CSCTRL_CL1(kQTMR_LoadOnComp2) | TMR_CSCTRL_CL2(kQTMR_LoadOnComp1));
    base->CHANNEL[channel].CSCTRL = reg;

    if (outputPolarity)
    {
        /* Invert the polarity */
        base->CHANNEL[channel].SCTRL |= TMR_SCTRL_OPS_MASK;
    }
    else
    {
        /* True polarity, no inversion */
        base->CHANNEL[channel].SCTRL &= ~TMR_SCTRL_OPS_MASK;
    }

    reg = base->CHANNEL[channel].CTRL;
    reg &= ~(TMR_CTRL_OUTMODE_MASK);
    /* Count until compare value is  reached and re-initialize the counter, toggle OFLAG output
     * using alternating compare register
     */
    reg |= (TMR_CTRL_LENGTH_MASK | TMR_CTRL_OUTMODE(kQTMR_ToggleOnAltCompareReg));
    base->CHANNEL[channel].CTRL = reg;

    return kStatus_Success;
}

/*!
 * brief Allows the user to count the source clock cycles until a capture event arrives.
 *
 * The count is stored in the capture register.
 *
 * param base            Quad Timer peripheral base address
 * param channel         Quad Timer channel number
 * param capturePin      Pin through which we receive the input signal to trigger the capture
 * param inputPolarity   true: invert polarity of the input signal, false: no inversion
 * param reloadOnCapture true: reload the counter when an input capture occurs, false: no reload
 * param captureMode     Specifies which edge of the input signal  triggers a capture
 */
void QTMR_SetupInputCapture(TMR_Type *base,
                            qtmr_channel_selection_t channel,
                            qtmr_input_source_t capturePin,
                            bool inputPolarity,
                            bool reloadOnCapture,
                            qtmr_input_capture_edge_t captureMode)
{
    uint16_t reg;

    /* Clear the prior value for the input source for capture */
    reg = base->CHANNEL[channel].CTRL & (~TMR_CTRL_SCS_MASK);

    /* Set the new input source */
    reg |= TMR_CTRL_SCS(capturePin);
    base->CHANNEL[channel].CTRL = reg;

    /* Clear the prior values for input polarity, capture mode. Set the external pin as input */
    reg = base->CHANNEL[channel].SCTRL & (~(TMR_SCTRL_IPS_MASK | TMR_SCTRL_CAPTURE_MODE_MASK | TMR_SCTRL_OEN_MASK));
    /* Set the new values */
    reg |= (TMR_SCTRL_IPS(inputPolarity) | TMR_SCTRL_CAPTURE_MODE(captureMode));
    base->CHANNEL[channel].SCTRL = reg;

    /* Setup if counter should reload when a capture occurs */
    if (reloadOnCapture)
    {
        base->CHANNEL[channel].CSCTRL |= TMR_CSCTRL_ROC_MASK;
    }
    else
    {
        base->CHANNEL[channel].CSCTRL &= ~TMR_CSCTRL_ROC_MASK;
    }
}

/*!
 * brief Enables the selected Quad Timer interrupts
 *
 * param base      Quad Timer peripheral base address
 * param channel   Quad Timer channel number
 * param mask      The interrupts to enable. This is a logical OR of members of the
 *                  enumeration ::qtmr_interrupt_enable_t
 */
void QTMR_EnableInterrupts(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask)
{
    uint16_t reg;

    reg = base->CHANNEL[channel].SCTRL;
    /* Compare interrupt */
    if (mask & kQTMR_CompareInterruptEnable)
    {
        reg |= TMR_SCTRL_TCFIE_MASK;
    }
    /* Overflow interrupt */
    if (mask & kQTMR_OverflowInterruptEnable)
    {
        reg |= TMR_SCTRL_TOFIE_MASK;
    }
    /* Input edge interrupt */
    if (mask & kQTMR_EdgeInterruptEnable)
    {
        /* Restriction: Do not set both SCTRL[IEFIE] and DMA[IEFDE] */
        base->CHANNEL[channel].DMA &= ~TMR_DMA_IEFDE_MASK;
        reg |= TMR_SCTRL_IEFIE_MASK;
    }
    base->CHANNEL[channel].SCTRL = reg;

    reg = base->CHANNEL[channel].CSCTRL;
    /* Compare 1 interrupt */
    if (mask & kQTMR_Compare1InterruptEnable)
    {
        reg |= TMR_CSCTRL_TCF1EN_MASK;
    }
    /* Compare 2 interrupt */
    if (mask & kQTMR_Compare2InterruptEnable)
    {
        reg |= TMR_CSCTRL_TCF2EN_MASK;
    }
    base->CHANNEL[channel].CSCTRL = reg;
}

/*!
 * brief Disables the selected Quad Timer interrupts
 *
 * param base     Quad Timer peripheral base addres
 * param channel  Quad Timer channel number
 * param mask The interrupts to enable. This is a logical OR of members of the
 *             enumeration ::qtmr_interrupt_enable_t
 */
void QTMR_DisableInterrupts(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask)
{
    uint16_t reg;

    reg = base->CHANNEL[channel].SCTRL;
    /* Compare interrupt */
    if (mask & kQTMR_CompareInterruptEnable)
    {
        reg &= ~TMR_SCTRL_TCFIE_MASK;
    }
    /* Overflow interrupt */
    if (mask & kQTMR_OverflowInterruptEnable)
    {
        reg &= ~TMR_SCTRL_TOFIE_MASK;
    }
    /* Input edge interrupt */
    if (mask & kQTMR_EdgeInterruptEnable)
    {
        reg &= ~TMR_SCTRL_IEFIE_MASK;
    }
    base->CHANNEL[channel].SCTRL = reg;

    reg = base->CHANNEL[channel].CSCTRL;
    /* Compare 1 interrupt */
    if (mask & kQTMR_Compare1InterruptEnable)
    {
        reg &= ~TMR_CSCTRL_TCF1EN_MASK;
    }
    /* Compare 2 interrupt */
    if (mask & kQTMR_Compare2InterruptEnable)
    {
        reg &= ~TMR_CSCTRL_TCF2EN_MASK;
    }
    base->CHANNEL[channel].CSCTRL = reg;
}

/*!
 * brief Gets the enabled Quad Timer interrupts
 *
 * param base    Quad Timer peripheral base address
 * param channel Quad Timer channel number
 *
 * return The enabled interrupts. This is the logical OR of members of the
 *         enumeration ::qtmr_interrupt_enable_t
 */
uint32_t QTMR_GetEnabledInterrupts(TMR_Type *base, qtmr_channel_selection_t channel)
{
    uint32_t enabledInterrupts = 0;
    uint16_t reg;

    reg = base->CHANNEL[channel].SCTRL;
    /* Compare interrupt */
    if (reg & TMR_SCTRL_TCFIE_MASK)
    {
        enabledInterrupts |= kQTMR_CompareFlag;
    }
    /* Overflow interrupt */
    if (reg & TMR_SCTRL_TOFIE_MASK)
    {
        enabledInterrupts |= kQTMR_OverflowInterruptEnable;
    }
    /* Input edge interrupt */
    if (reg & TMR_SCTRL_IEFIE_MASK)
    {
        enabledInterrupts |= kQTMR_EdgeInterruptEnable;
    }

    reg = base->CHANNEL[channel].CSCTRL;
    /* Compare 1 interrupt */
    if (reg & TMR_CSCTRL_TCF1EN_MASK)
    {
        enabledInterrupts |= kQTMR_Compare1InterruptEnable;
    }
    /* Compare 2 interrupt */
    if (reg & TMR_CSCTRL_TCF2EN_MASK)
    {
        enabledInterrupts |= kQTMR_Compare2InterruptEnable;
    }

    return enabledInterrupts;
}

/*!
 * brief Gets the Quad Timer status flags
 *
 * param base     Quad Timer peripheral base address
 * param channel  Quad Timer channel number
 *
 * return The status flags. This is the logical OR of members of the
 *         enumeration ::qtmr_status_flags_t
 */
uint32_t QTMR_GetStatus(TMR_Type *base, qtmr_channel_selection_t channel)
{
    uint32_t statusFlags = 0;
    uint16_t reg;

    reg = base->CHANNEL[channel].SCTRL;
    /* Timer compare flag */
    if (reg & TMR_SCTRL_TCF_MASK)
    {
        statusFlags |= kQTMR_CompareFlag;
    }
    /* Timer overflow flag */
    if (reg & TMR_SCTRL_TOF_MASK)
    {
        statusFlags |= kQTMR_OverflowFlag;
    }
    /* Input edge flag */
    if (reg & TMR_SCTRL_IEF_MASK)
    {
        statusFlags |= kQTMR_EdgeFlag;
    }

    reg = base->CHANNEL[channel].CSCTRL;
    /* Compare 1 flag */
    if (reg & TMR_CSCTRL_TCF1_MASK)
    {
        statusFlags |= kQTMR_Compare1Flag;
    }
    /* Compare 2 flag */
    if (reg & TMR_CSCTRL_TCF2_MASK)
    {
        statusFlags |= kQTMR_Compare2Flag;
    }

    return statusFlags;
}

/*!
 * brief Clears the Quad Timer status flags.
 *
 * param base     Quad Timer peripheral base address
 * param channel  Quad Timer channel number
 * param mask The status flags to clear. This is a logical OR of members of the
 *             enumeration ::qtmr_status_flags_t
 */
void QTMR_ClearStatusFlags(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask)
{
    uint16_t reg;

    reg = base->CHANNEL[channel].SCTRL;
    /* Timer compare flag */
    if (mask & kQTMR_CompareFlag)
    {
        reg &= ~TMR_SCTRL_TCF_MASK;
    }
    /* Timer overflow flag */
    if (mask & kQTMR_OverflowFlag)
    {
        reg &= ~TMR_SCTRL_TOF_MASK;
    }
    /* Input edge flag */
    if (mask & kQTMR_EdgeFlag)
    {
        reg &= ~TMR_SCTRL_IEF_MASK;
    }
    base->CHANNEL[channel].SCTRL = reg;

    reg = base->CHANNEL[channel].CSCTRL;
    /* Compare 1 flag */
    if (mask & kQTMR_Compare1Flag)
    {
        reg &= ~TMR_CSCTRL_TCF1_MASK;
    }
    /* Compare 2 flag */
    if (mask & kQTMR_Compare2Flag)
    {
        reg &= ~TMR_CSCTRL_TCF2_MASK;
    }
    base->CHANNEL[channel].CSCTRL = reg;
}

/*!
 * brief Sets the timer period in ticks.
 *
 * Timers counts from initial value till it equals the count value set here. The counter
 * will then reinitialize to the value specified in the Load register.
 *
 * note
 * 1. This function will write the time period in ticks to COMP1 or COMP2 register
 *    depending on the count direction
 * 2. User can call the utility macros provided in fsl_common.h to convert to ticks
 * 3. This function supports cases, providing only primary source clock without secondary source clock.
 *
 * param base     Quad Timer peripheral base address
 * param channel  Quad Timer channel number
 * param ticks Timer period in units of ticks
 */
void QTMR_SetTimerPeriod(TMR_Type *base, qtmr_channel_selection_t channel, uint16_t ticks)
{
    /* Set the length bit to reinitialize the counters on a match */
    base->CHANNEL[channel].CTRL |= TMR_CTRL_LENGTH_MASK;

    if (base->CHANNEL[channel].CTRL & TMR_CTRL_DIR_MASK)
    {
        /* Counting down */
        base->CHANNEL[channel].COMP2 = ticks;
    }
    else
    {
        /* Counting up */
        base->CHANNEL[channel].COMP1 = ticks;
    }
}

/*!
 * brief Enable the Quad Timer DMA.
 *
 * param base     Quad Timer peripheral base address
 * param channel  Quad Timer channel number
 * param mask     The DMA to enable. This is a logical OR of members of the
 *                  enumeration ::qtmr_dma_enable_t
 */
void QTMR_EnableDma(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask)
{
    uint16_t reg;

    reg = base->CHANNEL[channel].DMA;
    /* Input Edge Flag DMA Enable */
    if (mask & kQTMR_InputEdgeFlagDmaEnable)
    {
        /* Restriction: Do not set both DMA[IEFDE] and SCTRL[IEFIE] */
        base->CHANNEL[channel].SCTRL &= ~TMR_SCTRL_IEFIE_MASK;
        reg |= TMR_DMA_IEFDE_MASK;
    }
    /* Comparator Preload Register 1 DMA Enable */
    if (mask & kQTMR_ComparatorPreload1DmaEnable)
    {
        reg |= TMR_DMA_CMPLD1DE_MASK;
    }
    /* Comparator Preload Register 2 DMA Enable */
    if (mask & kQTMR_ComparatorPreload2DmaEnable)
    {
        reg |= TMR_DMA_CMPLD2DE_MASK;
    }
    base->CHANNEL[channel].DMA = reg;
}

/*!
 * brief Disable the Quad Timer DMA.
 *
 * param base     Quad Timer peripheral base address
 * param channel  Quad Timer channel number
 * param mask     The DMA to enable. This is a logical OR of members of the
 *                  enumeration ::qtmr_dma_enable_t
 */
void QTMR_DisableDma(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t mask)
{
    uint16_t reg;

    reg = base->CHANNEL[channel].DMA;
    /* Input Edge Flag DMA Enable */
    if (mask & kQTMR_InputEdgeFlagDmaEnable)
    {
        reg &= ~TMR_DMA_IEFDE_MASK;
    }
    /* Comparator Preload Register 1 DMA Enable */
    if (mask & kQTMR_ComparatorPreload1DmaEnable)
    {
        reg &= ~TMR_DMA_CMPLD1DE_MASK;
    }
    /* Comparator Preload Register 2 DMA Enable */
    if (mask & kQTMR_ComparatorPreload2DmaEnable)
    {
        reg &= ~TMR_DMA_CMPLD2DE_MASK;
    }
    base->CHANNEL[channel].DMA = reg;
}
