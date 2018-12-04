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

#include "py/nlr.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "shared-bindings/touchio/TouchIn.h"
#include "supervisor/shared/translate.h"

#include "tick.h"

bool touch_enabled = false;

static uint16_t get_raw_reading(touchio_touchin_obj_t *self) {
    return 4242;
}

void common_hal_touchio_touchin_construct(touchio_touchin_obj_t* self,
        const mcu_pin_obj_t *pin) {
//    if (!pin->has_touch) {
        mp_raise_ValueError(translate("Invalid pin"));
//    }
//    claim_pin(pin);
}

bool common_hal_touchio_touchin_deinited(touchio_touchin_obj_t* self) {
    return self->config.pin == NO_PIN;
}

void common_hal_touchio_touchin_deinit(touchio_touchin_obj_t* self) {
    if (common_hal_touchio_touchin_deinited(self)) {
        return;
    }

    reset_pin_number(self->config.pin);
    self->config.pin = NO_PIN;
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

void common_hal_touchio_touchin_set_threshold(touchio_touchin_obj_t *self, uint16_t new_threshold) {
    self->threshold = new_threshold;
}
