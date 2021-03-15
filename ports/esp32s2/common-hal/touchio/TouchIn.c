/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "shared-bindings/touchio/TouchIn.h"

#include "py/runtime.h"
#include "peripherals/touch.h"

static uint16_t get_raw_reading(touchio_touchin_obj_t *self) {
    uint32_t touch_value;
    touch_pad_read_raw_data(self->pin->touch_channel, &touch_value);
    if (touch_value > UINT16_MAX) {
        return UINT16_MAX;
    }
    return touch_value;
}

void common_hal_touchio_touchin_construct(touchio_touchin_obj_t *self,
    const mcu_pin_obj_t *pin) {
    if (pin->touch_channel == TOUCH_PAD_MAX) {
        mp_raise_ValueError(translate("Invalid pin"));
    }
    claim_pin(pin);

    // initialize touchpad
    peripherals_touch_init(pin->touch_channel);

    // wait for touch data to reset
    mp_hal_delay_ms(10);

    // Set a "touched" threshold not too far above the initial value.
    // For simple finger touch, the values may vary as much as a factor of two,
    // but for touches using fruit or other objects, the difference is much less.

    self->pin = pin;
    self->threshold = get_raw_reading(self) + 100;
}

bool common_hal_touchio_touchin_deinited(touchio_touchin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_touchio_touchin_deinit(touchio_touchin_obj_t *self) {
    if (common_hal_touchio_touchin_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->touch_channel);
    self->pin = NULL;
}

bool common_hal_touchio_touchin_get_value(touchio_touchin_obj_t *self) {
    return get_raw_reading(self) > self->threshold;
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
