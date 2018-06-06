/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <pwm.h>

#include "ets_alt_task.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "mpconfigport.h"
#include "shared-bindings/pulseio/PulseOut.h"

#define NO_CHANNEL (255)

static uint16_t *pulse_buffer = NULL;
static volatile uint16_t pulse_index = 0;
static uint16_t pulse_length;
static volatile uint32_t current_compare = 0;

void pulseout_set(pulseio_pulseout_obj_t *self, bool state) {
    // XXX double kludge
    //uint32_t duty = state ? pwm_get_period() * 11 : 0;
    uint32_t duty = state ? 1000 : 500;
    pwm_set_duty(duty, self->channel);
    pwm_start();
}

void pulseout_interrupt_handler(void *data) {
    pulseio_pulseout_obj_t *self = data;

    if (pulse_buffer == NULL || self->channel == NO_CHANNEL) return;
    if (pulse_index >= pulse_length) return;
    pulse_index++;
    pulseout_set(self, pulse_index % 2 == 0);

    os_timer_arm(&self->timer, pulse_buffer[pulse_index], 0);
}

void pulseout_reset() {
    pulse_buffer = NULL;
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t* self,
                                            const pulseio_pwmout_obj_t* carrier) {
    self->channel = carrier->channel;
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t* self) {
    return self->channel == NO_CHANNEL;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t* self) {
    os_timer_disarm(&self->timer);
    self->channel = NO_CHANNEL;
    pulseout_set(self, true);
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t* self, uint16_t* pulses, uint16_t length) {
    if (pulse_buffer != NULL) {
        mp_raise_RuntimeError("Another send is already active");
    }
    pulse_buffer = pulses;
    pulse_index = 0;
    pulse_length = length;

    os_timer_disarm(&self->timer);
    os_timer_setfn(&self->timer, pulseout_interrupt_handler, self);
    os_timer_arm(&self->timer, pulse_buffer[0], 0);
    pulseout_set(self, true);

    // XXX in the circumstances, is it worth messing with os_timer?
    // it isn't especially accurate anyway ...
    // might it not be simpler to just call mp_hal_delay_us() a lot?
    while(pulse_index < length) {
        ets_loop_iter();
    }

    os_timer_disarm(&self->timer);
    pulseout_set(self, false);

    pulse_buffer = NULL;
}
