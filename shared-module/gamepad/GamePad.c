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

#include "py/mpstate.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/gamepad/GamePad.h"
#include "supervisor/shared/tick.h"

void common_hal_gamepad_gamepad_init(gamepad_obj_t *gamepad,
    const mp_obj_t pins[], size_t n_pins) {
    for (size_t i = 0; i < 8; ++i) {
        gamepad->pins[i] = NULL;
    }
    gamepad->pulls = 0;
    for (size_t i = 0; i < n_pins; ++i) {
        digitalio_digitalinout_obj_t *pin = MP_OBJ_TO_PTR(pins[i]);
        if (common_hal_digitalio_digitalinout_get_direction(pin) !=
            DIRECTION_INPUT) {
            common_hal_digitalio_digitalinout_switch_to_input(pin, PULL_UP);
        }
        digitalio_pull_t pull = common_hal_digitalio_digitalinout_get_pull(pin);
        if (pull == PULL_NONE) {
            common_hal_digitalio_digitalinout_set_pull(pin, PULL_UP);
        }
        if (pull != PULL_DOWN) {
            gamepad->pulls |= 1 << i;
        }
        gamepad->pins[i] = pin;
    }
    gamepad->last = 0;
}

void common_hal_gamepad_gamepad_deinit(gamepad_obj_t *self) {
    MP_STATE_VM(gamepad_singleton) = NULL;
    supervisor_disable_tick();
}
