/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2018 Nick Moore for Adafruit Industries
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

#include <string.h>
#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "shared-bindings/touchio/TouchIn.h"
#include "shared-bindings/microcontroller/Pin.h"

// This is a capacitive touch sensing routine using a single digital
// pin.  The pin should be connected to the sensing pad, and to ground
// via a 1Mohm or thereabout drain resistor.  When a reading is taken,
// the pin's capacitance is charged by setting it to a digital output
// 'high' for a few microseconds, and then it is changed to a high
// impedance input.  We measure how long it takes to discharge through
// the resistor (around 50us), using a busy-waiting loop, and average
// over N_SAMPLES cycles to reduce the effects of noise.

#define N_SAMPLES 10
#define TIMEOUT_TICKS 10000

static uint16_t get_raw_reading(touchio_touchin_obj_t *self) {

    uint16_t ticks = 0;

    for (uint16_t i = 0; i < N_SAMPLES; i++) {
        // set pad to digital output high for 10us to charge it

        common_hal_digitalio_digitalinout_switch_to_output(self->digitalinout, true, DRIVE_MODE_PUSH_PULL);
        mp_hal_delay_us(10);

        // set pad back to an input and take some samples

        common_hal_digitalio_digitalinout_switch_to_input(self->digitalinout, PULL_NONE);

        while (common_hal_digitalio_digitalinout_get_value(self->digitalinout)) {
            if (ticks >= TIMEOUT_TICKS) {
                return TIMEOUT_TICKS;
            }
            ticks++;
        }
    }
    return ticks;
}

void common_hal_touchio_touchin_construct(touchio_touchin_obj_t *self, const mcu_pin_obj_t *pin) {
    common_hal_mcu_pin_claim(pin);
    self->digitalinout = m_new_obj(digitalio_digitalinout_obj_t);
    self->digitalinout->base.type = &digitalio_digitalinout_type;

    common_hal_digitalio_digitalinout_construct(self->digitalinout, pin);

    uint16_t raw_reading = get_raw_reading(self);
    if (raw_reading == TIMEOUT_TICKS) {
        mp_raise_ValueError(translate("No pulldown on pin; 1Mohm recommended"));
    }
    self->threshold = raw_reading * 1.05 + 100;
}

bool common_hal_touchio_touchin_deinited(touchio_touchin_obj_t *self) {
    return self->digitalinout == MP_OBJ_NULL;
}

void common_hal_touchio_touchin_deinit(touchio_touchin_obj_t *self) {
    if (common_hal_touchio_touchin_deinited(self)) {
        return;
    }

    common_hal_digitalio_digitalinout_deinit(self->digitalinout);
    self->digitalinout = MP_OBJ_NULL;
}

void touchin_reset() {
}

bool common_hal_touchio_touchin_get_value(touchio_touchin_obj_t *self) {
    uint16_t reading = get_raw_reading(self);
    return reading > self->threshold;
}

uint16_t common_hal_touchio_touchin_get_raw_value(touchio_touchin_obj_t *self) {
    return get_raw_reading(self);
}

uint16_t common_hal_touchio_touchin_get_threshold(touchio_touchin_obj_t *self) {
    return self->threshold;
}

void common_hal_touchio_touchin_set_threshold(touchio_touchin_obj_t *self,
    uint16_t new_threshold) {
    self->threshold = new_threshold;
}
