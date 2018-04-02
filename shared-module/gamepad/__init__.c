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

#include "__init__.h"
#include "GamePad.h"

#include "shared-bindings/digitalio/DigitalInOut.h"


void gamepad_tick(void) {
    if (!gamepad_singleton) {
        return;
    }
    uint8_t gamepad_current = 0;
    for (int i=0; i<8; ++i) {
        digitalio_digitalinout_obj_t* pin = gamepad_singleton->pins[i];
        if (!pin) {
            break;
        }
        digitalio_pull_t pull = common_hal_digitalio_digitalinout_get_pull(pin);
        bool value = common_hal_digitalio_digitalinout_get_value(pin);
        if ((pull == PULL_UP && !value) || (pull == PULL_DOWN && value)) {
            gamepad_current |= 1 << i;
        }
    }
    gamepad_singleton->pressed |= gamepad_singleton->last & gamepad_current;
    gamepad_singleton->last = gamepad_current;
}

void gamepad_reset(void) {
    gamepad_singleton = NULL;
}
