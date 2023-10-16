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
#include "common-hal/pwmio/PWMOut.h"
#include "shared-bindings/pwmio/PWMOut.h"

#include STM32_HAL_H
#include "shared-bindings/microcontroller/Pin.h"

#include "timers.h"

// Bitmask of channels taken.
STATIC uint8_t tim_channels_taken[TIM_BANK_ARRAY_LEN];
// Initial frequency timer is set to.
STATIC uint32_t tim_frequencies[TIM_BANK_ARRAY_LEN];
STATIC uint8_t never_reset_tim[TIM_BANK_ARRAY_LEN];
STATIC TIM_HandleTypeDef *active_handles[TIM_BANK_ARRAY_LEN];

STATIC uint32_t timer_get_internal_duty(uint16_t duty, uint32_t period) {
    // duty cycle is duty/0xFFFF fraction x (number of pulses per period)
    return (duty * period) / 0xffff;
}

STATIC bool timer_get_optimal_divisors(uint32_t *period, uint32_t *prescaler,
    uint32_t frequency, uint32_t source_freq) {
    // Find the largest possible period supported by this frequency
    *prescaler = 0;
    for (uint32_t i = 1; i <= 0xffff; i++) {
        *period = source_freq / (i * frequency);
        if (*period <= 0xffff && *period >= 2) {
            *prescaler = i;
            break;
        }
    }
    // Return success or failure.
    return *prescaler != 0;
}

void pwmout_reset(void) {
    for (int i = 0; i < TIM_BANK_ARRAY_LEN; i++) {
        if (active_handles[i] == NULL) {
            continue;
        }
        for (int c = 0; c < 8; c++) {
            if ((never_reset_tim[i] & (1 << c)) != 0 ||
                (tim_channels_taken[i] & (1 << c)) == 0) {
                continue;
            }
            HAL_TIM_PWM_Stop(active_handles[i], c);
        }
        // TODO: Actually shut down individual channels and PWM.
        if (never_reset_tim[i] != 0) {
            continue;
        }
        tim_channels_taken[i] = 0x00;
        tim_frequencies[i] = 0;
        stm_peripherals_timer_free(mcu_tim_banks[i]);
        HAL_TIM_PWM_DeInit(active_handles[i]);
        active_handles[i] = NULL;
    }
}

pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint16_t duty,
    uint32_t frequency,
    bool variable_frequency) {
    // Default error is no timer at all on pin.
    pwmout_result_t last_failure = PWMOUT_INVALID_PIN;
    bool first_time_setup = true;

    uint8_t tim_index;
    uint8_t tim_channel_index;

    self->tim = NULL;
    for (uint i = 0; i < MP_ARRAY_SIZE(mcu_tim_pin_list); i++) {
        const mcu_tim_pin_obj_t *tim = &mcu_tim_pin_list[i];
        tim_index = tim->tim_index;
        tim_channel_index = tim->channel_index;

        // if pin is same
        if (tim->pin == pin) {
            // check if the timer has a channel active, or is reserved by main timer system
            if (tim_index < TIM_BANK_ARRAY_LEN && tim_channels_taken[tim_index] != 0) {
                // Timer has already been reserved by an internal module
                if (stm_peripherals_timer_is_reserved(mcu_tim_banks[tim_index])) {
                    last_failure = PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
                    continue; // keep looking
                }
                // is it the same channel? (or all channels reserved by a var-freq)
                if (tim_channels_taken[tim_index] & (1 << tim_channel_index)) {
                    last_failure = PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
                    continue; // keep looking, might be another viable option
                }
                // If the frequencies are the same it's ok
                if (tim_frequencies[tim_index] != frequency) {
                    last_failure = PWMOUT_INVALID_FREQUENCY_ON_PIN;
                    continue; // keep looking
                }
                // you can't put a variable frequency on a partially reserved timer
                if (variable_frequency) {
                    last_failure = PWMOUT_VARIABLE_FREQUENCY_NOT_AVAILABLE;
                    continue; // keep looking
                }
                first_time_setup = false; // skip setting up the timer
            }
            // No problems taken, so set it up
            self->tim = tim;
            break;
        }
    }

    TIM_TypeDef *TIMx;

    // handle valid/invalid timer instance
    if (self->tim != NULL) {
        // create instance
        TIMx = mcu_tim_banks[tim_index];
        // reserve timer/channel
        if (variable_frequency) {
            // Take all the channels.
            tim_channels_taken[tim_index] = 0x0F;
        } else {
            tim_channels_taken[tim_index] |= 1 << tim_channel_index;
        }
        tim_frequencies[tim_index] = frequency;
        stm_peripherals_timer_reserve(TIMx);
    } else {
        // no match found
        return last_failure;
    }

    uint32_t prescaler = 0; // prescaler is 15 bit
    uint32_t period = 0; // period is 16 bit
    uint32_t source_freq = stm_peripherals_timer_get_source_freq(TIMx);
    if (!timer_get_optimal_divisors(&period, &prescaler, frequency, source_freq)) {
        return PWMOUT_INVALID_FREQUENCY;
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = self->tim->altfn_index;
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);
    self->pin = pin;

    tim_clock_enable(1 << tim_index);

    // translate channel into handle value: TIM_CHANNEL_1, _2, _3, _4.
    self->channel = 4 * tim_channel_index;

    // Timer init
    self->handle.Instance = TIMx;
    self->handle.Init.Period = period - 1;
    self->handle.Init.Prescaler = prescaler - 1;
    self->handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    self->handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    self->handle.Init.RepetitionCounter = 0;
    self->handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    // only run init if this is the first instance of this timer
    if (first_time_setup) {
        if (HAL_TIM_PWM_Init(&self->handle) != HAL_OK) {
            return PWMOUT_INITIALIZATION_ERROR;
        }
        active_handles[tim_index] = &self->handle;
    }

    // Channel/PWM init
    self->chan_handle.OCMode = TIM_OCMODE_PWM1;
    self->chan_handle.Pulse = timer_get_internal_duty(duty, period);
    self->chan_handle.OCPolarity = TIM_OCPOLARITY_HIGH;
    self->chan_handle.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&self->handle, &self->chan_handle, self->channel) != HAL_OK) {
        return PWMOUT_INITIALIZATION_ERROR;
    }
    if (HAL_TIM_PWM_Start(&self->handle, self->channel) != HAL_OK) {
        return PWMOUT_INITIALIZATION_ERROR;
    }

    self->variable_frequency = variable_frequency;
    self->frequency = frequency;
    self->duty_cycle = duty;
    self->period = period;

    return PWMOUT_OK;
}

void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self) {
    for (size_t i = 0; i < TIM_BANK_ARRAY_LEN; i++) {
        if (mcu_tim_banks[i] == self->handle.Instance) {
            never_reset_tim[i] = true;
            common_hal_never_reset_pin(self->pin);
            break;
        }
    }
}

bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self) {
    return self->tim == NULL;
}

void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        return;
    }
    // var freq shuts down entire timer, others just their channel
    if (self->variable_frequency) {
        tim_channels_taken[self->tim->tim_index] = 0x00;
    } else {
        tim_channels_taken[self->tim->tim_index] &= ~(1 << self->tim->channel_index);
        HAL_TIM_PWM_Stop(&self->handle, self->channel);
    }
    common_hal_reset_pin(self->pin);

    never_reset_tim[self->tim->tim_index] &= ~(1 << self->tim->channel_index);

    // if reserved timer has no active channels, we can disable it
    if (tim_channels_taken[self->tim->tim_index] == 0) {
        tim_frequencies[self->tim->tim_index] = 0x00;
        HAL_TIM_PWM_DeInit(&self->handle);
        active_handles[self->tim->tim_index] = NULL;
        stm_peripherals_timer_free(self->handle.Instance);
    }

    self->tim = NULL;
}

void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self, uint16_t duty) {
    uint32_t internal_duty_cycle = timer_get_internal_duty(duty, self->period);
    __HAL_TIM_SET_COMPARE(&self->handle, self->channel, internal_duty_cycle);
    self->duty_cycle = duty;
}

uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self) {
    return self->duty_cycle;
}

void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self, uint32_t frequency) {
    // don't halt setup for the same frequency
    if (frequency == self->frequency) {
        return;
    }

    uint32_t prescaler = 0;
    uint32_t period = 0;
    uint32_t source_freq = stm_peripherals_timer_get_source_freq(self->handle.Instance);
    timer_get_optimal_divisors(&period, &prescaler, frequency, source_freq);

    // shut down
    HAL_TIM_PWM_Stop(&self->handle, self->channel);

    // Only change altered values
    self->handle.Init.Period = period - 1;
    self->handle.Init.Prescaler = prescaler - 1;

    // restart everything, adjusting for new speed
    if (HAL_TIM_PWM_Init(&self->handle) != HAL_OK) {
        mp_raise_RuntimeError(translate("timer re-init"));
    }

    self->chan_handle.Pulse = timer_get_internal_duty(self->duty_cycle, period);

    if (HAL_TIM_PWM_ConfigChannel(&self->handle, &self->chan_handle, self->channel) != HAL_OK) {
        mp_raise_RuntimeError(translate("channel re-init"));
    }
    if (HAL_TIM_PWM_Start(&self->handle, self->channel) != HAL_OK) {
        mp_raise_RuntimeError(translate("PWM restart"));
    }

    tim_frequencies[self->tim->tim_index] = frequency;
    self->frequency = frequency;
    self->period = period;
}

uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t *self) {
    return self->frequency;
}

bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t *self) {
    return self->variable_frequency;
}

const mcu_pin_obj_t *common_hal_pwmio_pwmout_get_pin(pwmio_pwmout_obj_t *self) {
    return self->pin;
}
