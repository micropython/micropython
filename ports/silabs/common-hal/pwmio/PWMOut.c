/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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
#include "py/runtime.h"
#include "common-hal/pwmio/PWMOut.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/microcontroller/Pin.h"

STATIC sl_pwm_instance_t pwm_handle[TIM_BANK_ARRAY_LEN];
STATIC bool never_reset_tim[TIM_BANK_ARRAY_LEN];

mcu_tim_pin_obj_t mcu_tim_list[TIM_BANK_ARRAY_LEN] = {
    TIM(TIMER0, 0, FN_TIMER0_CC0, NULL),
    TIM(TIMER1, 0, FN_TIMER1_CC0, NULL),
    TIM(TIMER2, 0, FN_TIMER2_CC0, NULL),
    TIM(TIMER3, 0, FN_TIMER3_CC0, NULL),
    TIM(TIMER4, 0, FN_TIMER4_CC0, NULL),
};

// Reset all pwm channel
void pwmout_reset(void) {
    uint8_t tim_index;
    for (tim_index = 0; tim_index < TIM_BANK_ARRAY_LEN; tim_index++) {
        mcu_tim_pin_obj_t *l_tim = &mcu_tim_list[tim_index];
        if (l_tim->pin != NULL) {
            sl_pwm_deinit(&pwm_handle[tim_index]);
        }
    }

}

// Create a PWM object associated with the given pin
pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint16_t duty,
    uint32_t frequency,
    bool variable_frequency) {
    uint8_t tim_num = TIM_BANK_ARRAY_LEN;
    uint8_t percent = (duty * 100) / 65535;
    sl_pwm_config_t pwm_config;
    pwm_config.frequency = frequency;
    pwm_config.polarity = PWM_ACTIVE_LOW;

    mcu_tim_pin_obj_t *l_tim;
    uint8_t tim_index;

    if (self->tim == NULL) {
        for (tim_index = 0; tim_index < tim_num; tim_index++) {
            l_tim = &mcu_tim_list[tim_index];

            if ((l_tim->pin == NULL && pin->function_list[l_tim->fn_index] == 1)
                || l_tim->pin == pin) {
                l_tim->pin = pin;
                self->tim = l_tim;
                self->handle = &pwm_handle[tim_index];
                break;
            }
        }
    }

    if (self->tim == NULL) {
        return PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
    }

    self->duty_cycle = duty;
    self->variable_frequency = variable_frequency;
    self->frequency = frequency;
    self->handle->port = pin->port;
    self->handle->pin = pin->number;
    self->handle->timer = self->tim->timer;
    self->handle->channel = self->tim->channel;
    self->tim->pin = pin;

    if (SL_STATUS_OK != sl_pwm_init(self->handle, &pwm_config)) {
        return PWMOUT_INITIALIZATION_ERROR;
    }

    sl_pwm_start(self->handle);
    sl_pwm_set_duty_cycle(self->handle, percent);

    common_hal_mcu_pin_claim(pin);
    return PWMOUT_OK;
}

// Mark pwm obj to never reset after reload
void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self) {

    uint8_t tim_index;
    for (tim_index = 0; tim_index < TIM_BANK_ARRAY_LEN; tim_index++) {
        if (&mcu_tim_list[tim_index] == self->tim) {
            never_reset_tim[tim_index] = true;
            common_hal_never_reset_pin(self->tim->pin);
            break;
        }
    }
}

// Pwm will be reset after reloading.
void common_hal_pwmio_pwmout_reset_ok(pwmio_pwmout_obj_t *self) {

    uint8_t tim_index;
    for (tim_index = 0; tim_index < TIM_BANK_ARRAY_LEN; tim_index++) {
        if (&mcu_tim_list[tim_index] == self->tim) {
            never_reset_tim[tim_index] = false;
            break;
        }
    }
}

// Check pwm obj status, deinited or not
bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self) {
    return self->tim == NULL;
}

// Deint pwm obj
void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self) {
    sl_pwm_deinit(self->handle);
    common_hal_reset_pin(self->tim->pin);
    mcu_tim_pin_obj_t *l_tim = self->tim;
    l_tim->pin = NULL;
}

// Set pwm duty cycle
void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self,
    uint16_t duty) {
    uint8_t percent = (duty * 100) / 65535;
    sl_pwm_set_duty_cycle(self->handle, percent);
    self->duty_cycle = duty;
}

// Get pwm duty cycle
uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self) {
    return self->duty_cycle;
}

// Set pwm frequency
void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self,
    uint32_t frequency) {
    sl_pwm_config_t pwm_config;
    pwm_config.frequency = frequency;
    pwm_config.polarity = PWM_ACTIVE_LOW;
    sl_pwm_init(self->handle, &pwm_config);
}

// Get pwm frequency
uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t *self) {
    return self->frequency;
}

// Check variable frequency
bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t *self) {
    return self->variable_frequency;
}

// Get pin value in pwmio obj
const mcu_pin_obj_t *common_hal_pwmio_pwmout_get_pin(pwmio_pwmout_obj_t *self) {
    return self->tim->pin;
}
