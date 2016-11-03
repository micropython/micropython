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
#include "shared-bindings/nativeio/PWMOut.h"

// Shared with pybpwm
extern bool pwm_inited;

void common_hal_nativeio_pwmout_construct(nativeio_pwmout_obj_t* self, const mcu_pin_obj_t* pin, uint16_t duty) {
    // start the PWM subsystem if it's not already running
    if (!pwm_inited) {
        pwm_init();
        pwm_inited = true;
    }

    self->channel = pwm_add(pin->gpio_number,
                            pin->peripheral,
                            pin->gpio_function);
    if (self->channel == -1) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "PWM not supported on pin %d", pin->gpio_number));
    }
}

extern void common_hal_nativeio_pwmout_deinit(nativeio_pwmout_obj_t* self) {
    pwm_delete(self->channel);
    pwm_start();
}

extern void common_hal_nativeio_pwmout_set_duty_cycle(nativeio_pwmout_obj_t* self, uint16_t duty) {
    // We get 16 bits of duty in but the underlying code is only ten bit.
    pwm_set_duty(duty >> 6, self->channel);
    pwm_start();
}

uint16_t common_hal_nativeio_pwmout_get_duty_cycle(nativeio_pwmout_obj_t* self) {
    return pwm_get_duty(self->channel) << 6;
}
