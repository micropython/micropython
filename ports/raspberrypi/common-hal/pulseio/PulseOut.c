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
#include "src/rp2040/hardware_structs/include/hardware/structs/pwm.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"
#include "src/common/pico_time/include/pico/time.h"

static uint8_t refcount = 0;
volatile alarm_id_t cur_alarm = 0;

void turn_off(uint8_t slice) {
    pwm_hw->slice[slice].ctr = 0;
    pwm_hw->slice[slice].cc = 0;
    pwm_hw->slice[slice].top = 0;
    pwm_hw->slice[slice].div = 1u << PWM_CH0_DIV_INT_LSB;
    pwm_hw->slice[slice].csr = PWM_CH0_CSR_EN_BITS;
    pwm_hw->slice[slice].csr = 0;
}

void pulse_finish(pulseio_pulseout_obj_t *self) {
    self->pulse_index++;
    // Turn pwm pin off by setting duty cyle to 1.
    common_hal_pwmio_pwmout_set_duty_cycle(self->carrier,1);
    if (self->pulse_index >= self->pulse_length) {
        return;
    }
    if (self->pulse_index % 2 == 0) {
        common_hal_pwmio_pwmout_set_duty_cycle(self->carrier,self->current_duty_cycle);
    }
    uint64_t delay = self->pulse_buffer[self->pulse_index];
    if (delay < self->min_pulse) {
        delay = self->min_pulse;
    }
    cur_alarm = 0;
    // if the alarm cannot be set, try again with a longer delay
    while (cur_alarm == 0) {
        cur_alarm = add_alarm_in_us(delay, pulseout_interrupt_handler, self, false);
        delay = delay + 1;
    }
}

int64_t pulseout_interrupt_handler(alarm_id_t id, void *user_data) {
    pulse_finish(user_data);
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
    self->carrier = (pwmio_pwmout_obj_t *)carrier;
    self->current_duty_cycle = common_hal_pwmio_pwmout_get_duty_cycle(self->carrier);
    pwm_set_enabled(self->carrier->slice,false);
    turn_off(self->carrier->slice);
    common_hal_pwmio_pwmout_set_duty_cycle(self->carrier,1);
    self->pin = self->carrier->pin->number;
    self->slice = self->carrier->slice;
    self->min_pulse = (1000000 / self->carrier->actual_frequency);
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
    self->pulse_buffer = pulses;
    self->pulse_index = 0;
    self->pulse_length = length;

    common_hal_pwmio_pwmout_set_duty_cycle(self->carrier,self->current_duty_cycle);
    pwm_set_enabled(self->slice,true);
    uint64_t delay = self->pulse_buffer[0];
    if (delay < self->min_pulse) {
        delay = self->min_pulse;
    }
    cur_alarm = 0;
    // if the alarm cannot be set, try again with a longer delay
    while (cur_alarm == 0) {
        cur_alarm = add_alarm_in_us(delay, pulseout_interrupt_handler, self, false);
        delay = delay + 1;
    }

    while (self->pulse_index < length) {
        // Do other things while we wait. The interrupts will handle sending the
        // signal.
        RUN_BACKGROUND_TASKS;
    }
    // Ensure pin is left low
    turn_off(self->slice);
    pwm_set_enabled(self->slice,false);
}
