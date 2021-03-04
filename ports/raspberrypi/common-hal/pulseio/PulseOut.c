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

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

#include "mpconfigport.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "supervisor/shared/translate.h"

static uint8_t refcount = 0;


static uint16_t *pulse_buffer = NULL;
static volatile uint16_t pulse_index = 0;
static uint16_t pulse_length;
static volatile uint32_t current_compare = 0;

void pulse_finish(void) {
    pulse_index++;

    // Always turn it off.
    if (pulse_index >= pulse_length) {
        return;
    }
    current_compare = (current_compare + pulse_buffer[pulse_index] * 3 / 4) & 0xffff;
}

void pulseout_reset() {
    refcount = 0;
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t* self,
                                            const pwmio_pwmout_obj_t* carrier,
                                            const mcu_pin_obj_t* pin,
                                            uint32_t frequency,
                                            uint16_t duty_cycle) {
        mp_raise_NotImplementedError(translate("Unsupported operation"));

    refcount++;

    self->pin = carrier->pin->number;

}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t* self) {
    return self->pin == NO_PIN;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t* self) {
    if (common_hal_pulseio_pulseout_deinited(self)) {
        return;
    }


    refcount--;
    self->pin = NO_PIN;
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t* self, uint16_t* pulses, uint16_t length) {
    pulse_buffer = pulses;
    pulse_index = 0;
    pulse_length = length;

    current_compare = pulses[0] * 3 / 4;

}
