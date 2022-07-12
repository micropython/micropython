/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP *
 * Copyright (c) 2021 Robert Hammelrath
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef PWM_BACKPORT_H
#define PWM_BACKPORT_H
#include "fsl_pwm.h"
#ifdef FSL_FEATURE_SOC_TMR_COUNT
#include "fsl_qtmr.h"
#endif

typedef struct _pwm_signal_param_u16
{
    pwm_channels_t pwmChannel; // PWM channel being configured; PWM A or PWM B
    uint16_t dutyCycle_u16;    // PWM pulse width, value should be between 0 to 65536
    uint16_t Center_u16;       // Center of the pulse, value should be between 0 to 65536
    pwm_level_select_t level;  // PWM output active level select */
    uint16_t deadtimeValue;    // The deadtime value; only used if channel pair is operating in complementary mode
} pwm_signal_param_u16_t;

#define PWM_FULL_SCALE  (65536UL)

void PWM_UpdatePwmDutycycle_u16(PWM_Type *base, pwm_submodule_t subModule,
    pwm_channels_t pwmSignal, uint16_t dutyCycle, uint16_t center);

void PWM_SetupPwm_u16(PWM_Type *base, pwm_submodule_t subModule, pwm_signal_param_u16_t *chnlParams,
    uint32_t pwmFreq_Hz, uint32_t srcClock_Hz, bool output_enable);

void PWM_SetupPwmx_u16(PWM_Type *base, pwm_submodule_t subModule,
    uint32_t pwmFreq_Hz, uint16_t duty_cycle, uint8_t invert, uint32_t srcClock_Hz);

#ifdef FSL_FEATURE_SOC_TMR_COUNT
status_t QTMR_SetupPwm_u16(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t pwmFreqHz,
    uint16_t dutyCycleU16, bool outputPolarity, uint32_t srcClock_Hz, bool is_init);
#endif // FSL_FEATURE_SOC_TMR_COUNT

#endif // PWM_BACKPORT_H
