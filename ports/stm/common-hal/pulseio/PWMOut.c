/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
 * Uses code from Micropython, Copyright (c) 2013-2016 Damien P. George
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
#include "common-hal/pulseio/PWMOut.h"
#include "shared-bindings/pulseio/PWMOut.h"
#include "supervisor/shared/translate.h"

#include "shared-bindings/microcontroller/__init__.h"
#include STM32_HAL_H
#include "common-hal/microcontroller/Pin.h"

#define ALL_CLOCKS 0xFFFF

STATIC uint8_t reserved_tim[TIM_BANK_ARRAY_LEN];
STATIC uint32_t tim_frequencies[TIM_BANK_ARRAY_LEN];
STATIC bool never_reset_tim[TIM_BANK_ARRAY_LEN];

STATIC void tim_clock_enable(uint16_t mask);
STATIC void tim_clock_disable(uint16_t mask);

// Get the frequency (in Hz) of the source clock for the given timer.
// On STM32F405/407/415/417 there are 2 cases for how the clock freq is set.
// If the APB prescaler is 1, then the timer clock is equal to its respective
// APB clock.  Otherwise (APB prescaler > 1) the timer clock is twice its
// respective APB clock.  See DM00031020 Rev 4, page 115.
STATIC uint32_t timer_get_source_freq(uint32_t tim_id) {
    uint32_t source, clk_div;
    if (tim_id == 1 || (8 <= tim_id && tim_id <= 11)) {
        // TIM{1,8,9,10,11} are on APB2
        source = HAL_RCC_GetPCLK2Freq();
        clk_div = RCC->CFGR & RCC_CFGR_PPRE2;
    } else {
        // TIM{2,3,4,5,6,7,12,13,14} are on APB1
        source = HAL_RCC_GetPCLK1Freq();
        clk_div = RCC->CFGR & RCC_CFGR_PPRE1;
    }
    if (clk_div != 0) {
        // APB prescaler for this timer is > 1
        source *= 2;
    }
    return source;
}

STATIC uint32_t timer_get_internal_duty(uint16_t duty, uint32_t period) {
    //duty cycle is duty/0xFFFF fraction x (number of pulses per period)
    return (duty*period) / ((1 << 16) - 1);
}

STATIC void timer_get_optimal_divisors(uint32_t*period, uint32_t*prescaler,
                                       uint32_t frequency, uint32_t source_freq) {
    //Find the largest possible period supported by this frequency
    for (int i = 0; i < (1 << 16); i++) {
        *period = source_freq / (i * frequency);
        if (*period < (1 << 16) && *period >= 2) {
            *prescaler = i;
            break;
        }
    }
    if (*prescaler == 0) {
        mp_raise_ValueError(translate("Invalid frequency supplied"));
    }
}

void pwmout_reset(void) {
    uint16_t never_reset_mask = 0x00;
    for (int i = 0; i < TIM_BANK_ARRAY_LEN; i++) {
        if (!never_reset_tim[i]) {
            reserved_tim[i] = 0x00;
            tim_frequencies[i] = 0x00;
        } else {
            never_reset_mask |= 1 << i;
        }
    }
    tim_clock_disable(ALL_CLOCKS & ~(never_reset_mask));
}

pwmout_result_t common_hal_pulseio_pwmout_construct(pulseio_pwmout_obj_t* self,
                                                    const mcu_pin_obj_t* pin,
                                                    uint16_t duty,
                                                    uint32_t frequency,
                                                    bool variable_frequency) {
    TIM_TypeDef * TIMx;
    uint8_t tim_num = MP_ARRAY_SIZE(mcu_tim_pin_list);
    bool tim_chan_taken = false;
    bool tim_taken_f_mismatch = false;
    bool var_freq_mismatch = false;
    bool first_time_setup = true;

    for (uint i = 0; i < tim_num; i++) {
        const mcu_tim_pin_obj_t * l_tim = &mcu_tim_pin_list[i];
        uint8_t l_tim_index = l_tim->tim_index - 1;
        uint8_t l_tim_channel = l_tim->channel_index - 1;

        //if pin is same
        if (l_tim->pin == pin) {
            //check if the timer has a channel active
            if (reserved_tim[l_tim_index] != 0) {
                //is it the same channel? (or all channels reserved by a var-freq)
                if (reserved_tim[l_tim_index] & 1 << (l_tim_channel)) {
                    tim_chan_taken = true;
                    continue; //keep looking, might be another viable option
                }
                //If the frequencies are the same it's ok
                if (tim_frequencies[l_tim_index] != frequency) {
                    tim_taken_f_mismatch = true;
                    continue; //keep looking
                }
                //you can't put a variable frequency on a partially reserved timer
                if (variable_frequency) {
                    var_freq_mismatch = true;
                    continue; //keep looking
                }
                first_time_setup = false; //skip setting up the timer
            }
            //No problems taken, so set it up
            self->tim = l_tim;
            break;
        }
    }

    //handle valid/invalid timer instance
    if (self->tim != NULL) {
        //create instance
        TIMx = mcu_tim_banks[self->tim->tim_index - 1];
        //reserve timer/channel
        if (variable_frequency) {
            reserved_tim[self->tim->tim_index - 1] = 0x0F;
        } else {
            reserved_tim[self->tim->tim_index - 1] |= 1 << (self->tim->channel_index - 1);
        }
        tim_frequencies[self->tim->tim_index - 1] = frequency;
    } else { //no match found
        if (tim_chan_taken) {
            mp_raise_ValueError(translate("No more timers available on this pin."));
        } else if (tim_taken_f_mismatch) {
            mp_raise_ValueError(translate("Frequency must match existing PWMOut using this timer"));
        } else if (var_freq_mismatch) {
            mp_raise_ValueError(translate("Cannot vary frequency on a timer that is already in use"));
        } else {
            mp_raise_ValueError(translate("Invalid pins for PWMOut"));
        }
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = self->tim->altfn_index;
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);

    tim_clock_enable(1 << (self->tim->tim_index - 1));

    //translate channel into handle value
    self->channel = 4 * (self->tim->channel_index - 1);

    uint32_t prescaler = 0; //prescaler is 15 bit
    uint32_t period = 0; //period is 16 bit
    timer_get_optimal_divisors(&period, &prescaler, frequency,
                            timer_get_source_freq(self->tim->tim_index));

    //Timer init
    self->handle.Instance = TIMx;
    self->handle.Init.Period = period - 1;
    self->handle.Init.Prescaler = prescaler - 1;
    self->handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    self->handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    self->handle.Init.RepetitionCounter = 0;

    //only run init if this is the first instance of this timer
    if (first_time_setup) {
        if (HAL_TIM_PWM_Init(&self->handle) != HAL_OK) {
            mp_raise_ValueError(translate("Could not initialize timer"));
        }
    }

    //Channel/PWM init
    self->chan_handle.OCMode = TIM_OCMODE_PWM1;
    self->chan_handle.Pulse = timer_get_internal_duty(duty, period);
    self->chan_handle.OCPolarity = TIM_OCPOLARITY_HIGH;
    self->chan_handle.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&self->handle, &self->chan_handle, self->channel) != HAL_OK) {
        mp_raise_ValueError(translate("Could not initialize channel"));
    }
    if (HAL_TIM_PWM_Start(&self->handle, self->channel) != HAL_OK) {
        mp_raise_ValueError(translate("Could not start PWM"));
    }

    self->variable_frequency = variable_frequency;
    self->frequency = frequency;
    self->duty_cycle = duty;
    self->period = period;

    return PWMOUT_OK;
}

void common_hal_pulseio_pwmout_never_reset(pulseio_pwmout_obj_t *self) {
    for (size_t i = 0; i < TIM_BANK_ARRAY_LEN; i++) {
        if (mcu_tim_banks[i] == self->handle.Instance) {
            never_reset_tim[i] = true;
            never_reset_pin_number(self->tim->pin->port, self->tim->pin->number);
            break;
        }
    }
}

void common_hal_pulseio_pwmout_reset_ok(pulseio_pwmout_obj_t *self) {
    for(size_t i = 0; i < TIM_BANK_ARRAY_LEN; i++) {
        if (mcu_tim_banks[i] == self->handle.Instance) {
            never_reset_tim[i] = false;
            break;
        }
    }
}

bool common_hal_pulseio_pwmout_deinited(pulseio_pwmout_obj_t* self) {
    return self->tim == NULL;
}

void common_hal_pulseio_pwmout_deinit(pulseio_pwmout_obj_t* self) {
    if (common_hal_pulseio_pwmout_deinited(self)) {
        return;
    }
    //var freq shuts down entire timer, others just their channel
    if (self->variable_frequency) {
        reserved_tim[self->tim->tim_index - 1] = 0x00;
    } else {
        reserved_tim[self->tim->tim_index - 1] &= ~(1 << self->tim->channel_index);
        HAL_TIM_PWM_Stop(&self->handle, self->channel);
    }
    reset_pin_number(self->tim->pin->port,self->tim->pin->number);
    self->tim = NULL;

    //if reserved timer has no active channels, we can disable it
    if (!reserved_tim[self->tim->tim_index - 1]) {
        tim_frequencies[self->tim->tim_index - 1] = 0x00;
        tim_clock_disable(1 << (self->tim->tim_index - 1));
    }
}

void common_hal_pulseio_pwmout_set_duty_cycle(pulseio_pwmout_obj_t* self, uint16_t duty) {
    uint32_t internal_duty_cycle = timer_get_internal_duty(duty, self->period);
    __HAL_TIM_SET_COMPARE(&self->handle, self->channel, internal_duty_cycle);
    self->duty_cycle = duty;
}

uint16_t common_hal_pulseio_pwmout_get_duty_cycle(pulseio_pwmout_obj_t* self) {
    return self->duty_cycle;
}

void common_hal_pulseio_pwmout_set_frequency(pulseio_pwmout_obj_t* self, uint32_t frequency) {
    //don't halt setup for the same frequency
    if (frequency == self->frequency) {
        return;
    }

    uint32_t prescaler = 0;
    uint32_t period = 0;
    timer_get_optimal_divisors(&period, &prescaler, frequency,
                                timer_get_source_freq(self->tim->tim_index));

    //shut down
    HAL_TIM_PWM_Stop(&self->handle, self->channel);

    //Only change altered values
    self->handle.Init.Period = period - 1;
    self->handle.Init.Prescaler = prescaler - 1;

    //restart everything, adjusting for new speed
    if (HAL_TIM_PWM_Init(&self->handle) != HAL_OK) {
        mp_raise_ValueError(translate("Could not re-init timer"));
    }

    self->chan_handle.Pulse = timer_get_internal_duty(self->duty_cycle, period);

    if (HAL_TIM_PWM_ConfigChannel(&self->handle, &self->chan_handle, self->channel) != HAL_OK) {
        mp_raise_ValueError(translate("Could not re-init channel"));
    }
    if (HAL_TIM_PWM_Start(&self->handle, self->channel) != HAL_OK) {
        mp_raise_ValueError(translate("Could not restart PWM"));
    }

    tim_frequencies[self->tim->tim_index - 1] = frequency;
    self->frequency = frequency;
    self->period = period;
}

uint32_t common_hal_pulseio_pwmout_get_frequency(pulseio_pwmout_obj_t* self) {
    return self->frequency;
}

bool common_hal_pulseio_pwmout_get_variable_frequency(pulseio_pwmout_obj_t* self) {
    return self->variable_frequency;
}

STATIC void tim_clock_enable(uint16_t mask) {
    #ifdef TIM1
    if (mask & (1 << 0)) {
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
    #endif
    #ifdef TIM2
    if (mask & (1 << 1)) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    #endif
    #ifdef TIM3
    if (mask & (1 << 2)) {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
    #endif
    #ifdef TIM4
    if (mask & (1 << 3)) {
        __HAL_RCC_TIM4_CLK_ENABLE();
    }
    #endif
    #ifdef TIM5
    if (mask & (1 << 4)) {
        __HAL_RCC_TIM5_CLK_ENABLE();
    }
    #endif
    //6 and 7 are reserved ADC timers
    #ifdef TIM8
    if (mask & (1 << 7)) {
        __HAL_RCC_TIM8_CLK_ENABLE();
    }
    #endif
    #ifdef TIM9
    if (mask & (1 << 8)) {
        __HAL_RCC_TIM9_CLK_ENABLE();
    }
    #endif
    #ifdef TIM10
    if (mask & (1 << 9)) {
        __HAL_RCC_TIM10_CLK_ENABLE();
    }
    #endif
    #ifdef TIM11
    if (mask & (1 << 10)) {
        __HAL_RCC_TIM11_CLK_ENABLE();
    }
    #endif
    #ifdef TIM12
    if (mask & (1 << 11)) {
        __HAL_RCC_TIM12_CLK_ENABLE();
    }
    #endif
    #ifdef TIM13
    if (mask & (1 << 12)) {
        __HAL_RCC_TIM13_CLK_ENABLE();
    }
    #endif
    #ifdef TIM14
    if (mask & (1 << 13)) {
        __HAL_RCC_TIM14_CLK_ENABLE();
    }
    #endif
}

STATIC void tim_clock_disable(uint16_t mask) {
    #ifdef TIM1
    if (mask & (1 << 0)) {
        __HAL_RCC_TIM1_CLK_DISABLE();
    }
    #endif
    #ifdef TIM2
    if (mask & (1 << 1)) {
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
    #endif
    #ifdef TIM3
    if (mask & (1 << 2)) {
        __HAL_RCC_TIM3_CLK_DISABLE();
    }
    #endif
    #ifdef TIM4
    if (mask & (1 << 3)) {
        __HAL_RCC_TIM4_CLK_DISABLE();
    }
    #endif
    #ifdef TIM5
    if (mask & (1 << 4)) {
        __HAL_RCC_TIM5_CLK_DISABLE();
    }
    #endif
    //6 and 7 are reserved ADC timers
    #ifdef TIM8
    if (mask & (1 << 7)) {
        __HAL_RCC_TIM8_CLK_DISABLE();
    }
    #endif
    #ifdef TIM9
    if (mask & (1 << 8)) {
        __HAL_RCC_TIM9_CLK_DISABLE();
    }
    #endif
    #ifdef TIM10
    if (mask & (1 << 9)) {
        __HAL_RCC_TIM10_CLK_DISABLE();
    }
    #endif
    #ifdef TIM11
    if (mask & (1 << 10)) {
        __HAL_RCC_TIM11_CLK_DISABLE();
    }
    #endif
    #ifdef TIM12
    if (mask & (1 << 11)) {
        __HAL_RCC_TIM12_CLK_DISABLE();
    }
    #endif
    #ifdef TIM13
    if (mask & (1 << 12)) {
        __HAL_RCC_TIM13_CLK_DISABLE();
    }
    #endif
    #ifdef TIM14
    if (mask & (1 << 13)) {
        __HAL_RCC_TIM14_CLK_DISABLE();
    }
    #endif
}
