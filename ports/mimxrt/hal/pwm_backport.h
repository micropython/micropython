/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * The MIT License (MIT)
 * Copyright (c) 2021 Robert Hammelrath
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef PWM_BACKPORT_H
#define PWM_BACKPORT_H

typedef struct _pwm_signal_param_u16
{
    pwm_channels_t pwmChannel; // PWM channel being configured; PWM A or PWM B
    uint16_t dutyCycle_u16   ; // PWM pulse width, value should be between 0 to 65536
    uint16_t Center_u16;       // Center of the pulse, value should be between 0 to 65536
    pwm_level_select_t level;  // PWM output active level select */
    uint16_t deadtimeValue;    // The deadtime value; only used if channel pair is operating in complementary mode
} pwm_signal_param_u16_t;

typedef enum _pwm_fault_channels {
    kPWM_faultchannel_0 = 0U,
    kPWM_faultchannel_1
} pwm_fault_channels_t;

#define PWM_FULL_SCALE  (65536UL)

void PWM_UpdatePwmDutycycle_u16(PWM_Type *base, pwm_submodule_t subModule,
    pwm_channels_t pwmSignal, uint16_t dutyCycle, uint16_t center);

void PWM_SetupPwm_u16(PWM_Type *base, pwm_submodule_t subModule, pwm_signal_param_u16_t *chnlParams,
    uint32_t pwmFreq_Hz, uint32_t srcClock_Hz);

void PWM_SetupPwmx_u16(PWM_Type *base, pwm_submodule_t subModule,
    uint32_t pwmFreq_Hz, uint16_t duty_cycle, uint8_t invert, uint32_t srcClock_Hz);

void PWM_SetupFaultDisableMap(PWM_Type *base, pwm_submodule_t subModule,
    pwm_channels_t pwmChannel, pwm_fault_channels_t pwm_fault_channels, uint16_t value);

status_t QTMR_SetupPwm_u16(TMR_Type *base, qtmr_channel_selection_t channel, uint32_t pwmFreqHz,
    uint16_t dutyCycleU16, bool outputPolarity, uint32_t srcClock_Hz);

#endif // PWM_BACKPORT_H