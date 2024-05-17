// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/keypad/Keys.h"
#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/supervisor/__init__.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"

static void keypad_keys_scan_now(void *self_in, mp_obj_t timestamp);
static size_t keys_get_key_count(void *self_in);

static keypad_scanner_funcs_t keys_funcs = {
    .scan_now = keypad_keys_scan_now,
    .get_key_count = keys_get_key_count,
};

void common_hal_keypad_keys_construct(keypad_keys_obj_t *self, mp_uint_t num_pins, const mcu_pin_obj_t *pins[], bool value_when_pressed, bool pull, mp_float_t interval, size_t max_events, uint8_t debounce_threshold) {
    mp_obj_t dios[num_pins];

    for (size_t i = 0; i < num_pins; i++) {
        digitalio_digitalinout_obj_t *dio =
            mp_obj_malloc(digitalio_digitalinout_obj_t, &digitalio_digitalinout_type);
        common_hal_digitalio_digitalinout_construct(dio, pins[i]);
        if (pull) {
            common_hal_digitalio_digitalinout_set_pull(dio, value_when_pressed ? PULL_DOWN : PULL_UP);
        }
        dios[i] = dio;
    }

    self->digitalinouts = mp_obj_new_tuple(num_pins, dios);
    self->value_when_pressed = value_when_pressed;
    self->funcs = &keys_funcs;

    keypad_construct_common((keypad_scanner_obj_t *)self, interval, max_events, debounce_threshold);

}

void common_hal_keypad_keys_deinit(keypad_keys_obj_t *self) {
    if (common_hal_keypad_deinited(self)) {
        return;
    }

    // Remove self from the list of active keypad scanners first.
    keypad_deregister_scanner((keypad_scanner_obj_t *)self);

    for (size_t key = 0; key < keys_get_key_count(self); key++) {
        common_hal_digitalio_digitalinout_deinit(self->digitalinouts->items[key]);
    }
    self->digitalinouts = MP_ROM_NONE;

    common_hal_keypad_deinit_core(self);
}

size_t keys_get_key_count(void *self_in) {
    keypad_keys_obj_t *self = self_in;
    return self->digitalinouts->len;
}

static void keypad_keys_scan_now(void *self_in, mp_obj_t timestamp) {
    keypad_keys_obj_t *self = self_in;
    size_t key_count = keys_get_key_count(self);

    for (mp_uint_t key_number = 0; key_number < key_count; key_number++) {
        // Get the current state.
        const bool current =
            common_hal_digitalio_digitalinout_get_value(self->digitalinouts->items[key_number]) ==
            self->value_when_pressed;

        // Record any transitions.
        if (keypad_debounce((keypad_scanner_obj_t *)self, key_number, current)) {
            keypad_eventqueue_record(self->events, key_number, current, timestamp);
        }
    }
}
