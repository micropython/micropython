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

#include "fsl_pwm.h"

#include "supervisor/shared/translate.h"
#include "periph.h"

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
// TODO
// #include "samd/pins.h"

// #undef ENABLE
//
// #  define _TCC_SIZE(unused, n) TCC ## n ## _SIZE,
// #  define TCC_SIZES         { REPEAT_MACRO(_TCC_SIZE, 0, TCC_INST_NUM) }
//
// static uint32_t tcc_periods[TCC_INST_NUM];
// static uint32_t tc_periods[TC_INST_NUM];
//
// uint32_t target_tcc_frequencies[TCC_INST_NUM];
// uint8_t tcc_refcount[TCC_INST_NUM];
//
//// This bitmask keeps track of which channels of a TCC are currently claimed.
// #ifdef SAMD21
// uint8_t tcc_channels[3];   // Set by pwmout_reset() to {0xf0, 0xfc, 0xfc} initially.
// #endif
// #ifdef SAMD51
// uint8_t tcc_channels[5];   // Set by pwmout_reset() to {0xc0, 0xf0, 0xf8, 0xfc, 0xfc} initially.
// #endif
//
// static uint8_t never_reset_tc_or_tcc[TC_INST_NUM + TCC_INST_NUM];

void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self) {
//    if (self->timer->is_tc) {
//        never_reset_tc_or_tcc[self->timer->index] += 1;
//    } else {
//        never_reset_tc_or_tcc[TC_INST_NUM + self->timer->index] += 1;
//    }
//
//    never_reset_pin_number(self->pin->number);
}

void common_hal_pwmio_pwmout_reset_ok(pwmio_pwmout_obj_t *self) {
//    if (self->timer->is_tc) {
//        never_reset_tc_or_tcc[self->timer->index] -= 1;
//    } else {
//        never_reset_tc_or_tcc[TC_INST_NUM + self->timer->index] -= 1;
//    }
}

void pwmout_reset(void) {
//    // Reset all timers
//    for (int i = 0; i < TCC_INST_NUM; i++) {
//        target_tcc_frequencies[i] = 0;
//        tcc_refcount[i] = 0;
//    }
//    Tcc *tccs[TCC_INST_NUM] = TCC_INSTS;
//    for (int i = 0; i < TCC_INST_NUM; i++) {
//        if (never_reset_tc_or_tcc[TC_INST_NUM + i] > 0) {
//            continue;
//        }
//        // Disable the module before resetting it.
//        if (tccs[i]->CTRLA.bit.ENABLE == 1) {
//            tccs[i]->CTRLA.bit.ENABLE = 0;
//            while (tccs[i]->SYNCBUSY.bit.ENABLE == 1) {
//            }
//        }
//        uint8_t mask = 0xff;
//        for (uint8_t j = 0; j < tcc_cc_num[i]; j++) {
//            mask <<= 1;
//        }
//        tcc_channels[i] = mask;
//        tccs[i]->CTRLA.bit.SWRST = 1;
//        while (tccs[i]->CTRLA.bit.SWRST == 1) {
//        }
//    }
//    Tc *tcs[TC_INST_NUM] = TC_INSTS;
//    for (int i = 0; i < TC_INST_NUM; i++) {
//        if (never_reset_tc_or_tcc[i] > 0) {
//            continue;
//        }
//        tcs[i]->COUNT16.CTRLA.bit.SWRST = 1;
//        while (tcs[i]->COUNT16.CTRLA.bit.SWRST == 1) {
//        }
//    }
}

// static uint8_t tcc_channel(const pin_timer_t* t) {
//    // For the SAMD51 this hardcodes the use of OTMX == 0x0, the output matrix mapping, which uses
//    // SAMD21-style modulo mapping.
//    return t->wave_output % tcc_cc_num[t->index];
// }

// bool channel_ok(const pin_timer_t* t) {
//    uint8_t channel_bit = 1 << tcc_channel(t);
//    return (!t->is_tc && ((tcc_channels[t->index] & channel_bit) == 0)) ||
//            t->is_tc;
// }

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

    const uint32_t pwm_count = sizeof(mcu_pwm_list) / sizeof(mcu_pwm_obj_t);

    for (uint32_t i = 0; i < pwm_count; ++i) {
        if (mcu_pwm_list[i].pin != pin) {
            continue;
        }

        self->pwm = &mcu_pwm_list[i];

        break;
    }

    if (self->pwm == NULL) {
        return PWMOUT_INVALID_PIN;
    }

    config_periph_pin(self->pwm);

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

    // pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle;
    pwmConfig.enableDebugMode = true;

    self->pulse_count = calculate_pulse_count(frequency, &self->prescaler);

    if (self->pulse_count == 0) {
        return PWMOUT_INVALID_FREQUENCY;
    }

    pwmConfig.prescale = self->prescaler;

    if (PWM_Init(self->pwm->pwm, self->pwm->submodule, &pwmConfig) == kStatus_Fail) {
        return PWMOUT_INVALID_PIN;
    }

    pwm_signal_param_t pwmSignal = {
        .pwmChannel = self->pwm->channel,
        .level = kPWM_HighTrue,
        .dutyCyclePercent = 0, // avoid an initial transient
        .deadtimeValue = 0, // allow 100% duty cycle
    };

    // Disable all fault inputs
    self->pwm->pwm->SM[self->pwm->submodule].DISMAP[0] = 0;
    self->pwm->pwm->SM[self->pwm->submodule].DISMAP[1] = 0;

    status_t status = PWM_SetupPwm(self->pwm->pwm, self->pwm->submodule, &pwmSignal, 1, kPWM_EdgeAligned, frequency, PWM_SRC_CLK_FREQ);

    if (status != kStatus_Success) {
        return PWMOUT_INITIALIZATION_ERROR;
    }
    PWM_SetPwmLdok(self->pwm->pwm, 1 << self->pwm->submodule, true);

    PWM_StartTimer(self->pwm->pwm, 1 << self->pwm->submodule);


    common_hal_pwmio_pwmout_set_duty_cycle(self, duty);

    return PWMOUT_OK;
}

bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        return;
    }

    common_hal_reset_pin(self->pin);
    self->pin = NULL;
}

void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self, uint16_t duty) {
    // we do not use PWM_UpdatePwmDutycycle because ...
    //  * it works in integer percents
    //  * it can't set the "X" duty cycle
    self->duty_cycle = duty;
    if (duty == 65535) {
        self->duty_scaled = self->pulse_count + 1;
    } else {
        self->duty_scaled = ((uint32_t)duty * self->pulse_count + self->pulse_count / 2) / 65535;
    }
    switch (self->pwm->channel) {
        case kPWM_PwmX:
            self->pwm->pwm->SM[self->pwm->submodule].VAL0 = 0;
            self->pwm->pwm->SM[self->pwm->submodule].VAL1 = self->duty_scaled;
            break;
        case kPWM_PwmA:
            self->pwm->pwm->SM[self->pwm->submodule].VAL2 = 0;
            self->pwm->pwm->SM[self->pwm->submodule].VAL3 = self->duty_scaled;
            break;
        case kPWM_PwmB:
            self->pwm->pwm->SM[self->pwm->submodule].VAL4 = 0;
            self->pwm->pwm->SM[self->pwm->submodule].VAL5 = self->duty_scaled;
    }
    PWM_SetPwmLdok(self->pwm->pwm, 1 << self->pwm->submodule, true);
}

uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self) {
    if (self->duty_cycle == 65535) {
        return 65535;
    }
    return ((uint32_t)self->duty_scaled * 65535 + 65535 / 2) / self->pulse_count;
}

void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self,
    uint32_t frequency) {

    int pulse_count = calculate_pulse_count(frequency, &self->prescaler);
    if (pulse_count == 0) {
        mp_raise_ValueError(translate("Invalid PWM frequency"));
    }

    self->pulse_count = pulse_count;

    // a small glitch can occur when adjusting the prescaler, from the setting
    // of CTRL just below to the setting of the Ldok register in
    // set_duty_cycle.
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
