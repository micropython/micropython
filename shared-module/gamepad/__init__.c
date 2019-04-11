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

#include "shared-bindings/digitalio/DigitalInOut.h"


void gamepad_tick(void) {
    static uint8_t last = 0;
    uint8_t current = 0;
    uint8_t bit = 1;
    digitalio_digitalinout_obj_t* data_pin;
    digitalio_digitalinout_obj_t* clock_pin;
    digitalio_digitalinout_obj_t* latch_pin;

    gamepad_obj_t* gamepad_singleton = MP_STATE_VM(gamepad_singleton);
    if (!gamepad_singleton) {
        return;
    }
    if (gamepad_singleton->pins[0]) {
        // buttons connected directly to pins
        for (int i = 0; i < 8; ++i) {
            digitalio_digitalinout_obj_t* pin = gamepad_singleton->pins[i];
            if (!pin) {
                break;
            }
            if (common_hal_digitalio_digitalinout_get_value(pin)) {
                current |= bit;
            }
            bit <<= 1;
        }
        current ^= gamepad_singleton->pulls;
    } else {
        // buttons connected to a shift register
        data_pin = gamepad_singleton->pins[1];
        clock_pin = gamepad_singleton->pins[2];
        latch_pin = gamepad_singleton->pins[3];

        common_hal_digitalio_digitalinout_set_value(latch_pin, 1);
        for (int i = 0; i < 8; ++i) {
            common_hal_digitalio_digitalinout_set_value(clock_pin, 0);
            if (common_hal_digitalio_digitalinout_get_value(data_pin)) {
                current |= bit;
            }
            common_hal_digitalio_digitalinout_set_value(clock_pin, 1);
            bit <<= 1;
        }
        common_hal_digitalio_digitalinout_set_value(latch_pin, 0);
    }
    gamepad_singleton->pressed |= last & current;
    last = current;
}

void gamepad_reset(void) {
    MP_STATE_VM(gamepad_singleton) = NULL;
}
