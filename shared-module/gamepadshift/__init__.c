/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft
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

#include "shared-module/gamepadshift/__init__.h"

#include "py/mpstate.h"
#include "shared-bindings/gamepadshift/GamePadShift.h"

void gamepadshift_tick(void) {
    void *singleton = MP_STATE_VM(gamepad_singleton);
    if (singleton == NULL || !mp_obj_is_type(MP_OBJ_FROM_PTR(singleton), &gamepadshift_type)) {
        return;
    }

    gamepadshift_obj_t *self = MP_OBJ_TO_PTR(singleton);
    uint8_t current = 0;
    uint8_t bit = 1;
    common_hal_digitalio_digitalinout_set_value(self->latch_pin, 1);
    for (int i = 0; i < 8; ++i) {
        common_hal_digitalio_digitalinout_set_value(self->clock_pin, 0);
        if (common_hal_digitalio_digitalinout_get_value(self->data_pin)) {
            current |= bit;
        }
        common_hal_digitalio_digitalinout_set_value(self->clock_pin, 1);
        bit <<= 1;
    }
    common_hal_digitalio_digitalinout_set_value(self->latch_pin, 0);
    self->pressed |= self->last & current;
    self->last = current;
}

void gamepadshift_reset(void) {
    MP_STATE_VM(gamepad_singleton) = NULL;
}
