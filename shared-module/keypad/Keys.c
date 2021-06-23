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
#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/keypad/__init__.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"

void common_hal_keypad_keys_construct(keypad_keys_obj_t *self, mp_uint_t num_pins, mcu_pin_obj_t *pins[], bool value_when_pressed, bool pull, mp_float_t interval, size_t max_events) {
    mp_obj_t dios[num_pins];

    for (size_t i = 0; i < num_pins; i++) {
        digitalio_digitalinout_obj_t *dio = m_new_obj(digitalio_digitalinout_obj_t);
        dio->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(dio, pins[i]);
        if (pull) {
            common_hal_digitalio_digitalinout_set_pull(dio, value_when_pressed ? PULL_DOWN : PULL_UP);
        }
        dios[i] = dio;
    }

    self->digitalinouts = mp_obj_new_tuple(num_pins, dios);
    self->currently_pressed = (bool *)gc_alloc(sizeof(bool) * num_pins, false, false);
    self->previously_pressed = (bool *)gc_alloc(sizeof(bool) * num_pins, false, false);
    self->value_when_pressed = value_when_pressed;

    self->interval_ticks = (mp_uint_t)(interval * 1024);   // interval * 1000 * (1024/1000)
    self->last_scan_ticks = port_get_raw_ticks(NULL);

    keypad_eventqueue_obj_t *events = m_new_obj(keypad_eventqueue_obj_t);
    events->base.type = &keypad_eventqueue_type;
    common_hal_keypad_eventqueue_construct(events, max_events);
    self->events = events;

    // Add self to the list of active keypad scanners.
    keypad_register_scanner((keypad_scanner_obj_t *)self);

    supervisor_enable_tick();
}

void common_hal_keypad_keys_deinit(keypad_keys_obj_t *self) {
    if (common_hal_keypad_keys_deinited(self)) {
        return;
    }

    // Remove self from the list of active keypad scanners first.
    keypad_deregister_scanner((keypad_scanner_obj_t *)self);

    for (size_t key = 0; key < common_hal_keypad_keys_get_key_count(self); key++) {
        common_hal_digitalio_digitalinout_deinit(self->digitalinouts->items[key]);
    }
    self->digitalinouts = MP_ROM_NONE;

}

bool common_hal_keypad_keys_deinited(keypad_keys_obj_t *self) {
    return self->digitalinouts == MP_ROM_NONE;
}

size_t common_hal_keypad_keys_get_key_count(keypad_keys_obj_t *self) {
    return self->digitalinouts->len;
}

mp_obj_t common_hal_keypad_keys_get_events(keypad_keys_obj_t *self) {
    return MP_OBJ_FROM_PTR(self->events);
}

void common_hal_keypad_keys_reset(keypad_keys_obj_t *self) {
    const size_t key_count = common_hal_keypad_keys_get_key_count(self);

    supervisor_acquire_lock(&keypad_scanners_linked_list_lock);
    memset(self->previously_pressed, false, key_count);
    memset(self->currently_pressed, false, key_count);
    supervisor_release_lock(&keypad_scanners_linked_list_lock);
}

void keypad_keys_scan(keypad_keys_obj_t *self) {
    uint64_t now = port_get_raw_ticks(NULL);
    if (now - self->last_scan_ticks < self->interval_ticks) {
        // Too soon. Wait longer to debounce.
        return;
    }

    self->last_scan_ticks = now;

    const size_t key_count = common_hal_keypad_keys_get_key_count(self);

    for (mp_uint_t key_number = 0; key_number < key_count; key_number++) {
        // Remember the previous up/down state.
        const bool previous = self->currently_pressed[key_number];
        self->previously_pressed[key_number] = previous;

        // Get the current state.
        const bool current =
            common_hal_digitalio_digitalinout_get_value(self->digitalinouts->items[key_number]) ==
            self->value_when_pressed;
        self->currently_pressed[key_number] = current;

        // Record any transitions.
        if (previous != current) {
            keypad_eventqueue_record(self->events, key_number, current);
        }
    }
}
