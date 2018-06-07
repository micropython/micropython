/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Radomir Dopieralski for Adafruit Industries
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

#include <stdbool.h>

#include "py/mpstate.h"
#include "__init__.h"
#include "GamePad.h"

#include "shared-bindings/digitalio/Pull.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/util.h"


void gamepad_init(size_t n_pins, const mp_obj_t* pins) {
    gamepad_obj_t* gamepad_singleton = MP_STATE_VM(gamepad_singleton);
    for (size_t i = 0; i < 8; ++i) {
        gamepad_singleton->pins[i] = NULL;
    }
    gamepad_singleton->pulls = 0;
    for (size_t i = 0; i < n_pins; ++i) {
        digitalio_digitalinout_obj_t *pin = MP_OBJ_TO_PTR(pins[i]);
        digitalio_direction_t direction = common_hal_digitalio_digitalinout_get_direction(pin);
        if (direction != DIRECTION_INPUT) {
            common_hal_digitalio_digitalinout_switch_to_input(pin, PULL_UP);
        }
        digitalio_pull_t pull = common_hal_digitalio_digitalinout_get_pull(pin);
        if (pull == PULL_NONE) {
            common_hal_digitalio_digitalinout_set_pull(pin, PULL_UP);
        }
        if (pull != PULL_DOWN) {
            gamepad_singleton->pulls |= 1 << i;
        }
        gamepad_singleton->pins[i] = pin;
    }
    gamepad_singleton->last = 0;
}
