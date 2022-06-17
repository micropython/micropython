/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP *
 * Copyright (c) 2021 Robert Hammelrath
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

// These are a few functions taken from the NXP-Lib
// for PWM, for
// - dealing with an u16 duty cycle setting,
// - setting the pulse center position, and
// - factoring out pure duty cycle change.

#include "py/runtime.h"
#include "hal/pwm_backport.h"

void PWM_UpdatePwmDutycycle_u16(
    PWM_Type *base, pwm_submodule_t subModule, pwm_channels_t pwmSignal, uint16_t dutyCycle, uint16_t Center_u16) {
    assert((uint16_t)pwmSignal < 2U);
    uint16_t pulseCnt = 0, pwmHighPulse = 0;
    uint16_t center;

    // check and confine bounds for Center_u16
    if ((Center_u16 + dutyCycle / 2) >= PWM_FULL_SCALE) {
        Center_u16 = PWM_FULL_SCALE - dutyCycle / 2 - 1;
    } else if (Center_u16 < (dutyCycle / 2)) {
        Center_u16 = dutyCycle / 2;
    }
    pulseCnt = base->SM[subModule].VAL1 + 1;
    // Calculate pulse width and center position
    pwmHighPulse = (pulseCnt * dutyCycle) / PWM_FULL_SCALE;
    center = (pulseCnt * Center_u16) / PWM_FULL_SCALE;

    // Setup the PWM dutycycle of channel A or B
    if (pwmSignal == kPWM_PwmA) {
        base->SM[subModule].VAL2 = center - pwmHighPulse / 2;
        base->SM[subModule].VAL3 = base->SM[subModule].VAL2 + pwmHighPulse;
    } else {
        base->SM[subModule].VAL4 = center - pwmHighPulse / 2;
        base->SM[subModule].VAL5 = base->SM[subModule].VAL4 + pwmHighPulse;
    }
}

void PWM_SetupPwm_u16(PWM_Type *base, pwm_submodule_t subModule, pwm_signal_param_u16_t *chnlParams,
    uint32_t pwmFreq_Hz, uint32_t srcClock_Hz, bool output_enable) {

    uint32_t pwmClock;
    uint16_t pulseCnt = 0;
    uint8_t polarityShift = 0, outputEnableShift = 0;

    // Divide the clock by the prescale value
    pwmClock = (srcClock_Hz / (1U << ((base->SM[subModule].CTRL & PWM_CTRL_PRSC_MASK) >> PWM_CTRL_PRSC_SHIFT)));
    pulseCnt = (pwmClock + (pwmFreq_Hz - 1) / 2) / pwmFreq_Hz;
    base->SM[subModule].INIT = 0;
    base->SM[subModule].VAL1 = pulseCnt - 1;

    // Set up the Registers VAL2..VAL5 controlling the duty cycle of channel A/B
    PWM_UpdatePwmDutycycle_u16(base, subModule, chnlParams->pwmChannel,
        chnlParams->dutyCycle_u16, chnlParams->Center_u16);

    // Setup register shift values based on the channel being configured.
    // Also setup the deadtime value
    if (chnlParams->pwmChannel == kPWM_PwmA) {
        polarityShift = PWM_OCTRL_POLA_SHIFT;
        outputEnableShift = PWM_OUTEN_PWMA_EN_SHIFT;
        base->SM[subModule].DTCNT0 = PWM_DTCNT0_DTCNT0(chnlParams->deadtimeValue);
    } else {
        polarityShift = PWM_OCTRL_POLB_SHIFT;
        outputEnableShift = PWM_OUTEN_PWMB_EN_SHIFT;
        base->SM[subModule].DTCNT1 = PWM_DTCNT1_DTCNT1(chnlParams->deadtimeValue);
    }

    // Setup signal active level
    if (chnlParams->level == kPWM_HighTrue) {
        base->SM[subModule].OCTRL &= ~(1U << polarityShift);
    } else {
        base->SM[subModule].OCTRL |= (1U << polarityShift);
    }
    // Enable PWM output
    if (output_enable) {
        base->OUTEN |= (1U << (outputEnableShift + subModule));
    }
}

void PWM_SetupPwmx_u16(PWM_Type *base, pwm_submodule_t subModule,
    uint32_t pwmFreq_Hz, uint16_t duty_cycle, uint8_t invert, uint32_t srcClock_Hz) {

    uint32_t pulseCnt;
    uint32_t pwmClock;

    // Divide the clock by the prescale value
    pwmClock = (srcClock_Hz / (1U << ((base->SM[subModule].CTRL & PWM_CTRL_PRSC_MASK) >> PWM_CTRL_PRSC_SHIFT)));
    pulseCnt = (pwmClock + (pwmFreq_Hz - 1) / 2) / pwmFreq_Hz;
    base->SM[subModule].INIT = 0;
    base->SM[subModule].VAL0 = ((uint32_t)duty_cycle * pulseCnt) / PWM_FULL_SCALE - 1;
    base->SM[subModule].VAL1 = pulseCnt - 1;

    base->SM[subModule].OCTRL = (base->SM[subModule].OCTRL & ~PWM_OCTRL_POLX_MASK) | PWM_OCTRL_POLX(!invert);

    base->OUTEN |= (1U << subModule);
}

#ifdef FSL_FEATURE_SOC_TMR_COUNT
status_t QTMR_SetupPwm_u16(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t pwmFreqHz,
    uint16_t dutyCycleU16, bool outputPolarity, uint32_t srcClock_Hz, bool is_init) {
    uint32_t periodCount, highCount, lowCount, reg;

    if (dutyCycleU16 >= PWM_FULL_SCALE) {
        // Invalid dutycycle
        return kStatus_Fail;
    }

    // Counter values to generate a PWM signal
    periodCount = ((srcClock_Hz + (pwmFreqHz - 1) / 2) / pwmFreqHz) - 2;
    highCount = (periodCount * dutyCycleU16) / PWM_FULL_SCALE;
    lowCount = periodCount - highCount;

    // Setup the compare registers for PWM output
    if (is_init == false) {
        base->CHANNEL[channel].COMP1 = lowCount;
        base->CHANNEL[channel].COMP2 = highCount;
    }

    // Setup the pre-load registers for PWM output
    base->CHANNEL[channel].CMPLD1 = lowCount;
    base->CHANNEL[channel].CMPLD2 = highCount;

    reg = base->CHANNEL[channel].CSCTRL;
    // Setup the compare load control for COMP1 and COMP2.
    // Load COMP1 when CSCTRL[TCF2] is asserted, load COMP2 when CSCTRL[TCF1] is asserted
    reg &= ~(TMR_CSCTRL_CL1_MASK | TMR_CSCTRL_CL2_MASK);
    reg |= (TMR_CSCTRL_CL1(kQTMR_LoadOnComp2) | TMR_CSCTRL_CL2(kQTMR_LoadOnComp1));
    base->CHANNEL[channel].CSCTRL = reg;

    // Set OFLAG pin for output mode
    base->CHANNEL[channel].SCTRL |= TMR_SCTRL_OEN_MASK;
    if (outputPolarity) {
        // Invert the polarity
        base->CHANNEL[channel].SCTRL |= TMR_SCTRL_OPS_MASK;
    } else {
        // True polarity, no inversion
        base->CHANNEL[channel].SCTRL &= ~TMR_SCTRL_OPS_MASK;
    }

    reg = base->CHANNEL[channel].CTRL;
    reg &= ~(TMR_CTRL_OUTMODE_MASK);
    // Count until compare value is  reached and re-initialize the counter, toggle OFLAG output
    // using alternating compare register
    reg |= (TMR_CTRL_LENGTH_MASK | TMR_CTRL_OUTMODE(kQTMR_ToggleOnAltCompareReg));
    base->CHANNEL[channel].CTRL = reg;

    return kStatus_Success;
}
#endif // FSL_FEATURE_SOC_TMR_COUNT
