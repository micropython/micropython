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
#include "shared-module/gamepadshift/GamePadShift.h"
#include "supervisor/shared/tick.h"

void common_hal_gamepadshift_gamepadshift_init(gamepadshift_obj_t *gamepadshift,
    digitalio_digitalinout_obj_t *clock_pin,
    digitalio_digitalinout_obj_t *data_pin,
    digitalio_digitalinout_obj_t *latch_pin) {
    common_hal_digitalio_digitalinout_switch_to_input(data_pin, PULL_NONE);
    gamepadshift->data_pin = data_pin;
    common_hal_digitalio_digitalinout_switch_to_output(clock_pin, 0,
        DRIVE_MODE_PUSH_PULL);
    gamepadshift->clock_pin = clock_pin;
    common_hal_digitalio_digitalinout_switch_to_output(latch_pin, 1,
        DRIVE_MODE_PUSH_PULL);
    gamepadshift->latch_pin = latch_pin;

    gamepadshift->last = 0;
}

void common_hal_gamepadshift_gamepadshift_deinit(gamepadshift_obj_t *gamepadshift) {
    MP_STATE_VM(gamepad_singleton) = NULL;
    supervisor_disable_tick();
}
