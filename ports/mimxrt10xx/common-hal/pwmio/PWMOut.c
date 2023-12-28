/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George
 * Copyright (c) 2019 Artur Pacholec
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

#include <stdint.h>

#include "py/runtime.h"
#include "common-hal/pwmio/PWMOut.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "sdk/drivers/pwm/fsl_pwm.h"

#include "periph.h"

static PWM_Type *const _flexpwms[] = PWM_BASE_PTRS;

// 4 bits for each submodule in each FlexPWM.
static uint16_t _pwm_never_reset[MP_ARRAY_SIZE(_flexpwms)];
// Bitmask of whether state machines are use for variable frequency.
static uint8_t _pwm_variable_frequency[MP_ARRAY_SIZE(_flexpwms)];
// Configured frequency for each submodule.
static uint32_t _pwm_sm_frequencies[MP_ARRAY_SIZE(_flexpwms)][FSL_FEATURE_PWM_SUBMODULE_COUNT];
// Channels use is tracked using the OUTEN register.

// The SDK gives use clocks per submodule but they all share the same value! So, ignore the
// submodule and only turn off the clock when no other submodules are in use.
static const clock_ip_name_t _flexpwm_clocks[][FSL_FEATURE_PWM_SUBMODULE_COUNT] = PWM_CLOCKS;

static void config_periph_pin(const mcu_pwm_obj_t *periph) {
    IOMUXC_SetPinMux(
        periph->pin->mux_reg, periph->mux_mode,
        periph->input_reg, periph->input_idx,
        periph->pin->cfg_reg,
        0);

    IOMUXC_SetPinConfig(0, 0, 0, 0,
        periph->pin->cfg_reg,
        IOMUXC_SW_PAD_CTL_PAD_HYS(0)
        | IOMUXC_SW_PAD_CTL_PAD_PUS(1)
        | IOMUXC_SW_PAD_CTL_PAD_PUE(1)
        | IOMUXC_SW_PAD_CTL_PAD_PKE(1)
        | IOMUXC_SW_PAD_CTL_PAD_ODE(0)
        | IOMUXC_SW_PAD_CTL_PAD_SPEED(1)
        | IOMUXC_SW_PAD_CTL_PAD_DSE(6)
        | IOMUXC_SW_PAD_CTL_PAD_SRE(0));
}

static uint16_t _outen_mask(pwm_submodule_t submodule, pwm_channels_t channel) {
    uint16_t outen_mask = 0;
    uint8_t sm_mask = 1 << submodule;
    switch (channel) {
        case kPWM_PwmX:
            outen_mask |= PWM_OUTEN_PWMX_EN(sm_mask);
            break;
        case kPWM_PwmA:
            outen_mask |= PWM_OUTEN_PWMA_EN(sm_mask);
            break;
        case kPWM_PwmB:
            outen_mask |= PWM_OUTEN_PWMB_EN(sm_mask);
            break;
    }
    return outen_mask;
}

void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self) {
    common_hal_never_reset_pin(self->pin);
    _pwm_never_reset[self->flexpwm_index] |= (1 << (self->pwm->submodule * 4 + self->pwm->channel));
}

STATIC void _maybe_disable_clock(uint8_t instance) {
    if ((_flexpwms[instance]->MCTRL & PWM_MCTRL_RUN_MASK) == 0) {
        CLOCK_DisableClock(_flexpwm_clocks[instance][0]);
    }
}

void reset_all_flexpwm(void) {
    for (size_t i = 1; i < MP_ARRAY_SIZE(_pwm_never_reset); i++) {
        PWM_Type *flexpwm = _flexpwms[i];
        for (size_t submodule = 0; submodule < FSL_FEATURE_PWM_SUBMODULE_COUNT; submodule++) {
            uint8_t sm_mask = 1 << submodule;
            for (size_t channel = 0; channel < 3; channel++) {
                uint16_t channel_mask = 0x1 << (submodule * 4 + channel);
                if ((_pwm_never_reset[i] & channel_mask) != 0) {
                    continue;
                }

                // Turn off the channel.
                flexpwm->OUTEN &= ~_outen_mask(submodule, channel);
            }
            uint16_t submodule_mask = 0xf << (submodule * 4);
            if ((_pwm_never_reset[i] & submodule_mask) != 0) {
                // Leave the submodule on since a channel is marked for never_reset.
                continue;
            }
            flexpwm->MCTRL &= ~(sm_mask << PWM_MCTRL_RUN_SHIFT);
            _pwm_variable_frequency[i] &= ~sm_mask;
            _pwm_sm_frequencies[i][submodule] = 0;
        }
        _maybe_disable_clock(i);
    }
}

#define PWM_SRC_CLK_FREQ CLOCK_GetFreq(kCLOCK_IpgClk)

static int calculate_pulse_count(uint32_t frequency, uint8_t *prescaler) {
    if (frequency > PWM_SRC_CLK_FREQ / 2) {
        return 0;
    }
    for (int shift = 0; shift < 8; shift++) {
        int pulse_count = PWM_SRC_CLK_FREQ / (1 << shift) / frequency;
        if (pulse_count >= 65535) {
            continue;
        }
        *prescaler = shift;
        return pulse_count;
    }
    return 0;
}

pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint16_t duty,
    uint32_t frequency,
    bool variable_frequency) {
    self->pin = pin;
    self->variable_frequency = variable_frequency;

    for (uint32_t i = 0; i < MP_ARRAY_SIZE(mcu_pwm_list); ++i) {
        if (mcu_pwm_list[i].pin != pin) {
            continue;
        }

        self->pwm = &mcu_pwm_list[i];

        break;
    }

    if (self->pwm == NULL) {
        return PWMOUT_INVALID_PIN;
    }

    PWM_Type *flexpwm = self->pwm->pwm;
    pwm_submodule_t submodule = self->pwm->submodule;
    uint16_t sm_mask = 1 << submodule;
    pwm_channels_t channel = self->pwm->channel;

    uint8_t flexpwm_index = 1;
    for (; flexpwm_index < MP_ARRAY_SIZE(_flexpwms); flexpwm_index++) {
        if (_flexpwms[flexpwm_index] == flexpwm) {
            break;
        }
    }
    self->flexpwm_index = flexpwm_index;

    uint16_t outen_mask = _outen_mask(submodule, channel);

    self->pulse_count = calculate_pulse_count(frequency, &self->prescaler);

    if (self->pulse_count == 0) {
        return PWMOUT_INVALID_FREQUENCY;
    }

    // The submodule is already running
    if (((flexpwm->MCTRL >> PWM_MCTRL_RUN_SHIFT) & sm_mask) != 0) {
        // Another output has claimed this submodule for variable frequency already.
        if ((_pwm_variable_frequency[flexpwm_index] & sm_mask) != 0) {
            return PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
        }

        // We want variable frequency but another class has already claim a fixed frequency.
        if (variable_frequency) {
            return PWMOUT_VARIABLE_FREQUENCY_NOT_AVAILABLE;
        }

        // Another pin is already using this output.
        if ((flexpwm->OUTEN & outen_mask) != 0) {
            return PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
        }

        if (frequency != _pwm_sm_frequencies[flexpwm_index][submodule]) {
            return PWMOUT_INVALID_FREQUENCY_ON_PIN;
        }

        // Submodule is already running at our target frequency and the output
        // is free.
    } else {
        pwm_config_t pwmConfig;

        /*
         * pwmConfig.enableDebugMode = false;
         * pwmConfig.enableWait = false;
         * pwmConfig.reloadSelect = kPWM_LocalReload;
         * pwmConfig.faultFilterCount = 0;
         * pwmConfig.faultFilterPeriod = 0;
         * pwmConfig.clockSource = kPWM_BusClock;
         * pwmConfig.prescale = kPWM_Prescale_Divide_1;
         * pwmConfig.initializationControl = kPWM_Initialize_LocalSync;
         * pwmConfig.forceTrigger = kPWM_Force_Local;
         * pwmConfig.reloadFrequency = kPWM_LoadEveryOportunity;
         * pwmConfig.reloadLogic = kPWM_ReloadImmediate;
         * pwmConfig.pairOperation = kPWM_Independent;
         */
        PWM_GetDefaultConfig(&pwmConfig);

        pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle;
        pwmConfig.enableWait = true;
        pwmConfig.enableDebugMode = true;

        pwmConfig.prescale = self->prescaler;

        if (PWM_Init(flexpwm, submodule, &pwmConfig) != kStatus_Success) {
            return PWMOUT_INITIALIZATION_ERROR;
        }

        // Disable all fault inputs
        flexpwm->SM[submodule].DISMAP[0] = 0;

        PWM_SetPwmLdok(flexpwm, sm_mask, false);
        flexpwm->SM[submodule].CTRL = PWM_CTRL_FULL_MASK | PWM_CTRL_PRSC(self->prescaler);
        flexpwm->SM[submodule].CTRL2 = PWM_CTRL2_INDEP_MASK | PWM_CTRL2_WAITEN_MASK | PWM_CTRL2_DBGEN_MASK;
        // Set the reload value to zero so we're in unsigned mode.
        flexpwm->SM[submodule].INIT = 0;
        // Set the top/reload value.
        flexpwm->SM[submodule].VAL1 = self->pulse_count;
        // Clear the other channels.
        flexpwm->SM[submodule].VAL0 = 0;
        flexpwm->SM[submodule].VAL2 = 0;
        flexpwm->SM[submodule].VAL3 = 0;
        flexpwm->SM[submodule].VAL4 = 0;
        flexpwm->SM[submodule].VAL5 = 0;
        PWM_SetPwmLdok(flexpwm, sm_mask, true);

        PWM_StartTimer(flexpwm, sm_mask);
        _pwm_sm_frequencies[flexpwm_index][submodule] = frequency;

        if (variable_frequency) {
            _pwm_variable_frequency[flexpwm_index] = sm_mask;
        }
    }

    common_hal_pwmio_pwmout_set_duty_cycle(self, duty);

    flexpwm->OUTEN |= outen_mask;

    // Configure the IOMUX once we know everything else is working.
    config_periph_pin(self->pwm);

    return PWMOUT_OK;
}

bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        return;
    }

    _pwm_never_reset[self->flexpwm_index] &= ~(1 << (self->pwm->submodule * 4 + self->pwm->channel));

    PWM_Type *flexpwm = self->pwm->pwm;
    pwm_submodule_t submodule = self->pwm->submodule;
    uint16_t sm_mask = 1 << submodule;

    // Reset the pin before we turn it off.
    common_hal_reset_pin(self->pin);
    self->pin = NULL;

    // Always disable the output.
    flexpwm->OUTEN &= ~_outen_mask(submodule, self->pwm->channel);

    uint16_t all_sm_channels = _outen_mask(submodule, kPWM_PwmX) | _outen_mask(submodule, kPWM_PwmA) | _outen_mask(submodule, kPWM_PwmB);

    // Turn off the submodule if it doesn't have any outputs active.
    if ((flexpwm->OUTEN & all_sm_channels) == 0) {
        // Deinit ourselves because the SDK turns off the clock to the whole FlexPWM on deinit.
        flexpwm->MCTRL &= ~(sm_mask << PWM_MCTRL_RUN_SHIFT);
        _pwm_variable_frequency[self->flexpwm_index] &= ~sm_mask;
        _pwm_sm_frequencies[self->flexpwm_index][submodule] = 0;
    }
    _maybe_disable_clock(self->flexpwm_index);
}

void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self, uint16_t duty) {
    // we do not use PWM_UpdatePwmDutycycle because ...
    //  * it works in integer percents
    //  * it can't set the "X" duty cycle
    // As mentioned in the setting up of the frequency code
    //      A - Uses VAL2 to turn on (0) and VAL3=duty to turn off
    //      B - Uses VAL4 to turn on (0) and VAL5 to turn off
    //      X - As mentioned above VAL1 turns off, but it's set to the timing for frequency. so
    //          VAL0 turns on, so we set it to VAL1 - duty

    self->duty_cycle = duty;
    PWM_Type *base = self->pwm->pwm;
    uint8_t sm_mask = 1 << self->pwm->submodule;
    uint16_t duty_scaled;
    if (duty == 65535) {
        // X channels can't do a full 100% duty cycle.
        if (self->pwm->channel == kPWM_PwmX) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_duty_cycle);
        }
        duty_scaled = self->pulse_count + 1;
    } else {
        duty_scaled = ((uint32_t)duty * self->pulse_count) / 65535;
    }
    PWM_SetPwmLdok(self->pwm->pwm, sm_mask, false);
    switch (self->pwm->channel) {
        case kPWM_PwmX:
            // PWM X Signals always having a falling edge at the reload value. (Otherwise we'd
            // change the PWM frequency.) So, we adjust the rising edge to get the correct duty
            // cycle.
            base->SM[self->pwm->submodule].VAL0 = self->pulse_count - duty_scaled;
            break;
        case kPWM_PwmA:
            // The other two channels always have their rising edge at 0 and vary their falling
            // edge.
            base->SM[self->pwm->submodule].VAL3 = duty_scaled;
            break;
        case kPWM_PwmB:
            base->SM[self->pwm->submodule].VAL5 = duty_scaled;
    }
    PWM_SetPwmLdok(self->pwm->pwm, sm_mask, true);
}

uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self) {
    return self->duty_cycle;
}

void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self,
    uint32_t frequency) {

    int pulse_count = calculate_pulse_count(frequency, &self->prescaler);
    if (pulse_count == 0) {
        mp_arg_error_invalid(MP_QSTR_frequency);
    }

    self->pulse_count = pulse_count;

    // a small glitch can occur when adjusting the prescaler, from the setting
    // of CTRL just below to the setting of the Ldok register in
    // set_duty_cycle.
    // Clear LDOK so that we can update the values.
    PWM_SetPwmLdok(self->pwm->pwm, 1 << self->pwm->submodule, false);
    uint32_t reg = self->pwm->pwm->SM[self->pwm->submodule].CTRL;
    reg &= ~(PWM_CTRL_PRSC_MASK);
    reg |= PWM_CTRL_PRSC(self->prescaler);
    self->pwm->pwm->SM[self->pwm->submodule].CTRL = reg;
    self->pwm->pwm->SM[self->pwm->submodule].VAL1 = self->pulse_count;

    // we need to recalculate the duty cycle.  As a side effect of this
    common_hal_pwmio_pwmout_set_duty_cycle(self, self->duty_cycle);
}

uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t *self) {
    return PWM_SRC_CLK_FREQ / self->pulse_count / (1 << self->prescaler);
}

bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t *self) {
    return self->variable_frequency;
}

const mcu_pin_obj_t *common_hal_pwmio_pwmout_get_pin(pwmio_pwmout_obj_t *self) {
    return self->pin;
}
