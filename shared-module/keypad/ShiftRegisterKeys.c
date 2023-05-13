/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbert for Adafruit Industries
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

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/keypad/ShiftRegisterKeys.h"
#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/supervisor/__init__.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"

static void shiftregisterkeys_scan_now(void *self, mp_obj_t timestamp);
static size_t shiftregisterkeys_get_key_count(void *self);

static keypad_scanner_funcs_t shiftregisterkeys_funcs = {
    .scan_now = shiftregisterkeys_scan_now,
    .get_key_count = shiftregisterkeys_get_key_count,
};

void common_hal_keypad_shiftregisterkeys_construct(keypad_shiftregisterkeys_obj_t *self, const mcu_pin_obj_t *clock_pin, const mcu_pin_obj_t *data_pin, const mcu_pin_obj_t *latch_pin, bool value_to_latch, size_t key_count, bool value_when_pressed, mp_float_t interval, size_t max_events) {

    digitalio_digitalinout_obj_t *clock = m_new_obj(digitalio_digitalinout_obj_t);
    clock->base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(clock, clock_pin);
    common_hal_digitalio_digitalinout_switch_to_output(clock, false, DRIVE_MODE_PUSH_PULL);
    self->clock = clock;

    digitalio_digitalinout_obj_t *data = m_new_obj(digitalio_digitalinout_obj_t);
    data->base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(data, data_pin);
    common_hal_digitalio_digitalinout_switch_to_input(data, PULL_NONE);
    self->data = data;

    digitalio_digitalinout_obj_t *latch = m_new_obj(digitalio_digitalinout_obj_t);
    latch->base.type = &digitalio_digitalinout_type;

    common_hal_digitalio_digitalinout_construct(latch, latch_pin);
    common_hal_digitalio_digitalinout_switch_to_output(latch, true, DRIVE_MODE_PUSH_PULL);
    self->latch = latch;
    self->value_to_latch = value_to_latch;

    self->value_when_pressed = value_when_pressed;
    self->key_count = key_count;
    self->funcs = &shiftregisterkeys_funcs;

    keypad_construct_common((keypad_scanner_obj_t *)self, interval, max_events);
}

void common_hal_keypad_shiftregisterkeys_deinit(keypad_shiftregisterkeys_obj_t *self) {
    if (common_hal_keypad_deinited(self)) {
        return;
    }

    // Remove self from the list of active keypad scanners first.
    keypad_deregister_scanner((keypad_scanner_obj_t *)self);


    common_hal_digitalio_digitalinout_deinit(self->clock);
    self->clock = MP_ROM_NONE;

    common_hal_digitalio_digitalinout_deinit(self->data);
    self->data = MP_ROM_NONE;

    common_hal_digitalio_digitalinout_deinit(self->latch);
    self->latch = MP_ROM_NONE;

    common_hal_keypad_deinit_core(self);
}

size_t shiftregisterkeys_get_key_count(void *self_in) {
    keypad_shiftregisterkeys_obj_t *self = self_in;
    return self->key_count;
}

static void shiftregisterkeys_scan_now(void *self_in, mp_obj_t timestamp) {
    keypad_shiftregisterkeys_obj_t *self = self_in;

    // Latch (freeze) the current state of the input pins.
    common_hal_digitalio_digitalinout_set_value(self->latch, self->value_to_latch);

    const size_t key_count = shiftregisterkeys_get_key_count(self);

    for (mp_uint_t key_number = 0; key_number < key_count; key_number++) {
        // Zero-th data appears on on the data pin immediately, without shifting.
        common_hal_digitalio_digitalinout_set_value(self->clock, false);

        // Remember the previous up/down state.
        const bool previous = self->currently_pressed[key_number];
        self->previously_pressed[key_number] = previous;

        // Get the current state.
        const bool current =
            common_hal_digitalio_digitalinout_get_value(self->data) == self->value_when_pressed;
        self->currently_pressed[key_number] = current;

        // Trigger a shift to get the next bit.
        common_hal_digitalio_digitalinout_set_value(self->clock, true);

        // Record any transitions.
        if (previous != current) {
            keypad_eventqueue_record(self->events, key_number, current, timestamp);
        }
    }

    // Start reading the input pins again.
    common_hal_digitalio_digitalinout_set_value(self->latch, !self->value_to_latch);
}
