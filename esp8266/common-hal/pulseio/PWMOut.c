/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include <stdio.h>
#include <stdint.h>

#include "esppwm.h"

#include "py/runtime.h"
#include "shared-bindings/pulseio/PWMOut.h"

#include "eagle_soc.h"
#include "c_types.h"
#include "gpio.h"

#define PWM_FREQ_MAX 1000

// Shared with pybpwm
extern bool pwm_inited;
bool first_channel_variable;

void pwmout_reset(void) {
    first_channel_variable = false;
}

void common_hal_pulseio_pwmout_construct(pulseio_pwmout_obj_t* self, const mcu_pin_obj_t* pin, uint16_t duty, uint32_t frequency,
        bool variable_frequency) {
    if (frequency > PWM_FREQ_MAX) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "Maximum PWM frequency is %dhz.", PWM_FREQ_MAX));
    } else if (frequency < 1) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "Minimum PWM frequency is 1hz."));
    }

    // start the PWM subsystem if it's not already running
    if (!pwm_inited) {
        pwm_init();
        pwm_inited = true;
        pwm_set_freq(frequency, 0);
        first_channel_variable = variable_frequency;
    } else if (first_channel_variable || pwm_get_freq(0) != frequency) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "Multiple PWM frequencies not supported. PWM already set to %dhz.", pwm_get_freq(0)));
    }

    self->channel = pwm_add(pin->gpio_number,
                            pin->peripheral,
                            pin->gpio_function);
    self->pin = pin;
    if (self->channel == -1) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "PWM not supported on pin %d", pin->gpio_number));
    }
}

bool common_hal_pulseio_pwmout_deinited(pulseio_pwmout_obj_t* self) {
    return self->pin == mp_const_none;
}

void common_hal_pulseio_pwmout_deinit(pulseio_pwmout_obj_t* self) {
    if (common_hal_pulseio_pwmout_deinited(self)) {
        return;
    }
    pwm_delete(self->channel);
    pwm_start();
    if (self->pin->gpio_number < 16) {
        uint32_t pin_mask = 1 << self->pin->gpio_number;
        gpio_output_set(0x0, 0x0, 0x0, pin_mask);
        PIN_FUNC_SELECT(self->pin->peripheral, 0);
        PIN_PULLUP_DIS(self->pin->peripheral);
    }
    self->pin = mp_const_none;
}

void common_hal_pulseio_pwmout_set_duty_cycle(pulseio_pwmout_obj_t* self, uint16_t duty) {
    // We get 16 bits of duty in but the underlying code is only ten bit.
    pwm_set_duty(duty >> 6, self->channel);
    pwm_start();
}

uint16_t common_hal_pulseio_pwmout_get_duty_cycle(pulseio_pwmout_obj_t* self) {
    return pwm_get_duty(self->channel) << 6;
}

void common_hal_pulseio_pwmout_set_frequency(pulseio_pwmout_obj_t* self, uint32_t frequency) {
    if (frequency > PWM_FREQ_MAX) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "Maximum PWM frequency is %dhz.", PWM_FREQ_MAX));
    } else if (frequency < 1) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "Minimum PWM frequency is 1hz."));
    }
    pwm_set_freq(frequency, 0);
}

uint32_t common_hal_pulseio_pwmout_get_frequency(pulseio_pwmout_obj_t* self) {
    return pwm_get_freq(0);
}

bool common_hal_pulseio_pwmout_get_variable_frequency(pulseio_pwmout_obj_t* self) {
    return first_channel_variable;
}
