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

#include "supervisor/shared/translate/translate.h"
#include "periph.h"

// Debug print support set to zero to enable debug printing
#define ENABLE_DEBUG_PRINTING 0


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

void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self) {
}

void common_hal_pwmio_pwmout_reset_ok(pwmio_pwmout_obj_t *self) {
}

void pwmout_reset(void) {
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

// ==========================================================
// Debug code
// ==========================================================
#if ENABLE_DEBUG_PRINTING
#define DBGPrintf mp_printf
extern void debug_print_flexpwm_registers(PWM_Type *base);

void debug_print_flexpwm_registers(PWM_Type *base) {
    mp_printf(&mp_plat_print,
        "\t\tPWM OUTEN:%x  MASK:%x  SWCOUT:%x DTSRCSEL:%x MCTRL:%x MCTRL2:%x FCTRL:%x FSTS:%x FFILT:%x FTST:%x FCTRL2:%x\n",
        base->OUTEN, base->MASK, base->SWCOUT, base->DTSRCSEL, base->MCTRL, base->MCTRL2, base->FCTRL,
        base->FSTS, base->FFILT, base->FTST, base->FCTRL2);
    for (uint8_t i = 0; i < 4; i++) {
        mp_printf(&mp_plat_print,
            "\t\t(%u) INIT:%x CTRL2:%x CTRL:%x VAL0:%x VAL1:%x VAL2:%x VAL3:%x VAL4:%x VAL5:%x OCTRL:%x DTCNT0:%x DTCNT1:%x  DISMAP: %x %x\n", i,
            base->SM[i].INIT, base->SM[i].CTRL2, base->SM[i].CTRL, base->SM[i].VAL0, base->SM[i].VAL1, base->SM[i].VAL2,
            base->SM[i].VAL3, base->SM[i].VAL4, base->SM[i].VAL5, base->SM[i].OCTRL, base->SM[i].DTCNT0, base->SM[i].DTCNT1,
            base->SM[i].DISMAP[0], base->SM[i].DISMAP[1]);
    }

}
#else
#define DBGPrintf(p,...)
inline void debug_print_flexpwm_registers(PWM_Type *base) {
}
#endif

pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint16_t duty,
    uint32_t frequency,
    bool variable_frequency) {
    self->pin = pin;
    self->variable_frequency = variable_frequency;

    const uint32_t pwm_count = sizeof(mcu_pwm_list) / sizeof(mcu_pwm_obj_t);

    DBGPrintf(&mp_plat_print, ">>> common_hal_pwmio_pwmout_construct called: pin: %p %u freq:%u duty:%u var:%u\n",
        self->pin->gpio, self->pin->number, frequency, duty, variable_frequency);

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

    DBGPrintf(&mp_plat_print, "\tFound in PWM List\n");

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

    DBGPrintf(&mp_plat_print, "\tCall PWM_Init\n");
    if (PWM_Init(self->pwm->pwm, self->pwm->submodule, &pwmConfig) == kStatus_Fail) {
        return PWMOUT_INVALID_PIN;
    }

    // Disable all fault inputs
    self->pwm->pwm->SM[self->pwm->submodule].DISMAP[0] = 0;
    self->pwm->pwm->SM[self->pwm->submodule].DISMAP[1] = 0;

    DBGPrintf(&mp_plat_print, "\tCall PWM_SetupPwm %p %x %u\n", self->pwm->pwm, self->pwm->submodule);
    // ========================================================================================================
    // Not calling the PWM_SetupPwm as it was setup to only work for PWM output on chan A and B but not X
    // I have done some experimenting, probably could try others, but again they do not work with X.
    // Most of the code checks to see if A if not, then it assume B.
    //
    // Instead I set it up to work similar to what the Teensy 4.x code does.
    //
    // That is we set the PWM_CTRL_FULL_MASK, which then uses  base->SM[submodule].VAL1  to control
    // when the timer is reset, so it sets up your cycle/frequency.  But then this implies that X channel
    // which uses 0, 1 has to be handled specially. So for the different channels:
    //      A - Uses VAL2 to turn on (0) and VAL3=duty to turn off
    //      B - Uses VAL4 to turn on (0) and VAL5 to turn off
    //      X - As mentioned above VAL1 turns off, but it's set to the timing for frequency. so
    //          VAL0 turns on, so we set it to VAL1 - duty
    //
    PWM_Type *base = self->pwm->pwm;
    uint8_t submodule = self->pwm->submodule;

    uint32_t mask = 1 << submodule;
    uint32_t olddiv = base->SM[submodule].VAL1 + 1;
    if (self->pulse_count != olddiv) {
        base->MCTRL |= PWM_MCTRL_CLDOK(mask);
        base->SM[submodule].CTRL = PWM_CTRL_PRSC_MASK | PWM_CTRL_PRSC(self->prescaler);
        base->SM[submodule].VAL1 = self->pulse_count - 1;
        base->SM[submodule].CTRL2 = PWM_CTRL2_INDEP_MASK | PWM_CTRL2_WAITEN_MASK | PWM_CTRL2_DBGEN_MASK;

        if (olddiv == 1) {
            base->SM[submodule].CTRL = PWM_CTRL_FULL_MASK;
            base->SM[submodule].VAL0 = 0;
            base->SM[submodule].VAL2 = 0;
            base->SM[submodule].VAL3 = 0;
            base->SM[submodule].VAL4 = 0;
            base->SM[submodule].VAL5 = 0;
        } else {
            base->SM[submodule].VAL0 = (base->SM[submodule].VAL0 * self->pulse_count) / olddiv;
            base->SM[submodule].VAL3 = (base->SM[submodule].VAL3 * self->pulse_count) / olddiv;
            base->SM[submodule].VAL5 = (base->SM[submodule].VAL5 * self->pulse_count) / olddiv;
        }
        base->MCTRL |= PWM_MCTRL_LDOK(mask);
    }
    debug_print_flexpwm_registers(self->pwm->pwm);

    PWM_SetPwmLdok(self->pwm->pwm, 1 << self->pwm->submodule, true);

    PWM_StartTimer(self->pwm->pwm, 1 << self->pwm->submodule);


    DBGPrintf(&mp_plat_print, "\tCall common_hal_pwmio_pwmout_set_duty_cycle\n");
    common_hal_pwmio_pwmout_set_duty_cycle(self, duty);

    DBGPrintf(&mp_plat_print, "\tReturn OK\n");
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
    // As mentioned in the setting up of the frequency code
    //      A - Uses VAL2 to turn on (0) and VAL3=duty to turn off
    //      B - Uses VAL4 to turn on (0) and VAL5 to turn off
    //      X - As mentioned above VAL1 turns off, but it's set to the timing for frequency. so
    //          VAL0 turns on, so we set it to VAL1 - duty

    DBGPrintf(&mp_plat_print, "common_hal_pwmio_pwmout_set_duty_cycle %u\n", duty);
    self->duty_cycle = duty;
    PWM_Type *base = self->pwm->pwm;
    uint8_t mask = 1 << self->pwm->submodule;
    if (duty == 65535) {
        self->duty_scaled = self->pulse_count + 1;
    } else {
        self->duty_scaled = ((uint32_t)duty * self->pulse_count + self->pulse_count / 2) / 65535;
    }
    switch (self->pwm->channel) {
        case kPWM_PwmX:
            base->SM[self->pwm->submodule].VAL0 = self->pulse_count - self->duty_scaled;
            base->OUTEN |= PWM_OUTEN_PWMX_EN(mask);
            break;
        case kPWM_PwmA:
            base->SM[self->pwm->submodule].VAL3 = self->duty_scaled;
            base->OUTEN |= PWM_OUTEN_PWMA_EN(mask);
            break;
        case kPWM_PwmB:
            base->SM[self->pwm->submodule].VAL5 = self->duty_scaled;
            base->OUTEN |= PWM_OUTEN_PWMB_EN(mask);
    }
    PWM_SetPwmLdok(self->pwm->pwm, 1 << self->pwm->submodule, true);

    debug_print_flexpwm_registers(self->pwm->pwm);

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
        mp_arg_error_invalid(MP_QSTR_frequency);
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

const mcu_pin_obj_t *common_hal_pwmio_pwmout_get_pin(pwmio_pwmout_obj_t *self) {
    return self->pin;
}
