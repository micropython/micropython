/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dave Putz for Adafruit Industries
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

#include "common-hal/pulseio/PulseOut.h"

#include <stdint.h>
#include "mpconfigport.h"
#include "py/runtime.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/pwmio/PWMOut.h"
#include "supervisor/shared/translate.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"
#include "src/common/pico_time/include/pico/time.h"

static uint8_t refcount = 0;
static uint16_t *pulse_buffer = NULL;
static volatile uint16_t pulse_index = 0;
static uint16_t pulse_length;
pwmio_pwmout_obj_t *pwmout_obj;
volatile uint16_t current_duty_cycle;

void pulse_finish(void) {
    pulse_index++;
    // Turn pwm pin off by setting duty cyle to 1.
    common_hal_pwmio_pwmout_set_duty_cycle(pwmout_obj,1);
    if (pulse_index >= pulse_length) {
        return;
    }
    add_alarm_in_us(pulse_buffer[pulse_index], pulseout_interrupt_handler, NULL, false);
    if (pulse_index % 2 == 0) {
        common_hal_pwmio_pwmout_set_duty_cycle(pwmout_obj,current_duty_cycle);
    }
}

int64_t pulseout_interrupt_handler(alarm_id_t id, void *user_data) {
    pulse_finish();
    return 0;
}

void pulseout_reset() {
    refcount = 0;
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const pwmio_pwmout_obj_t *carrier,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle) {

    refcount++;
    pwmout_obj = (pwmio_pwmout_obj_t *)carrier;
    current_duty_cycle = common_hal_pwmio_pwmout_get_duty_cycle(pwmout_obj);
    self->pin = carrier->pin->number;
    self->slice = carrier->slice;
    pwm_set_enabled(pwmout_obj->slice,false);
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self) {
    return self->pin == NO_PIN;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self) {
    if (common_hal_pulseio_pulseout_deinited(self)) {
        return;
    }
    refcount--;
    self->pin = NO_PIN;
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t length) {
    pulse_buffer = pulses;
    pulse_index = 0;
    pulse_length = length;

    add_alarm_in_us(pulses[0], pulseout_interrupt_handler, NULL, false);
    common_hal_pwmio_pwmout_set_duty_cycle(pwmout_obj,current_duty_cycle);
    pwm_set_enabled(pwmout_obj->slice,true);

    while (pulse_index < length) {
        // Do other things while we wait. The interrupts will handle sending the
        // signal.
        RUN_BACKGROUND_TASKS;
    }
    // Short delay to give pin time to settle before disabling PWM
    common_hal_mcu_delay_us(25);
    pwm_set_enabled(pwmout_obj->slice,false);
}
