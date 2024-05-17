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

void common_hal_keypad_shiftregisterkeys_construct(keypad_shiftregisterkeys_obj_t *self, const mcu_pin_obj_t *clock_pin, mp_uint_t num_data_pins, const mcu_pin_obj_t *data_pins[], const mcu_pin_obj_t *latch_pin, bool value_to_latch, mp_uint_t num_key_counts, size_t key_counts[], bool value_when_pressed, mp_float_t interval, size_t max_events, uint8_t debounce_threshold) {

    digitalio_digitalinout_obj_t *clock =
        mp_obj_malloc(digitalio_digitalinout_obj_t, &digitalio_digitalinout_type);
    common_hal_digitalio_digitalinout_construct(clock, clock_pin);
    common_hal_digitalio_digitalinout_switch_to_output(clock, false, DRIVE_MODE_PUSH_PULL);
    self->clock = clock;

    digitalio_digitalinout_obj_t *latch = m_new_obj(digitalio_digitalinout_obj_t);
    latch->base.type = &digitalio_digitalinout_type;

    common_hal_digitalio_digitalinout_construct(latch, latch_pin);
    common_hal_digitalio_digitalinout_switch_to_output(latch, true, DRIVE_MODE_PUSH_PULL);
    self->latch = latch;

    mp_obj_t dios[num_data_pins];

    for (size_t i = 0; i < num_data_pins; i++) {
        digitalio_digitalinout_obj_t *dio = m_new_obj(digitalio_digitalinout_obj_t);
        dio->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(dio, data_pins[i]);
        common_hal_digitalio_digitalinout_switch_to_input(dio, PULL_NONE);
        dios[i] = dio;
    }

    // Allocate a tuple object with the data pins
    self->data_pins = mp_obj_new_tuple(num_data_pins, dios);

    self->key_counts = (mp_uint_t *)m_malloc(sizeof(mp_uint_t) * num_key_counts);
    self->num_key_counts = num_key_counts;

    // copy to an m_malloc() and on the fly record pin with largest Shift register
    mp_uint_t max = 0;

    for (mp_uint_t i = 0; i < self->num_key_counts; i++) {
        mp_uint_t cnt = key_counts[i];

        if (cnt > max) {
            max = cnt;
        }

        self->key_counts[i] = cnt;
    }

    self->max_key_count = max;

    self->value_to_latch = value_to_latch;
    self->value_when_pressed = value_when_pressed;
    self->funcs = &shiftregisterkeys_funcs;

    keypad_construct_common((keypad_scanner_obj_t *)self, interval, max_events, debounce_threshold);
}

void common_hal_keypad_shiftregisterkeys_deinit(keypad_shiftregisterkeys_obj_t *self) {
    if (common_hal_keypad_deinited(self)) {
        return;
    }

    // Remove self from the list of active keypad scanners first.
    keypad_deregister_scanner((keypad_scanner_obj_t *)self);


    common_hal_digitalio_digitalinout_deinit(self->clock);
    self->clock = MP_ROM_NONE;

    common_hal_digitalio_digitalinout_deinit(self->latch);
    self->latch = MP_ROM_NONE;

    for (size_t key = 0; key < self->data_pins->len; key++) {
        common_hal_digitalio_digitalinout_deinit(self->data_pins->items[key]);
    }
    self->data_pins = MP_ROM_NONE;
    self->key_counts = MP_ROM_NONE;

    common_hal_keypad_deinit_core(self);
}

size_t shiftregisterkeys_get_key_count(void *self_in) {
    keypad_shiftregisterkeys_obj_t *self = self_in;

    size_t total = 0;

    for (mp_uint_t i = 0; i < self->num_key_counts; i++) {
        total += self->key_counts[i];
    }

    return total;
}

static void shiftregisterkeys_scan_now(void *self_in, mp_obj_t timestamp) {
    keypad_shiftregisterkeys_obj_t *self = self_in;

    // Latch (freeze) the current state of the input pins.
    common_hal_digitalio_digitalinout_set_value(self->latch, self->value_to_latch);

    // Scan for max_key_count bit
    for (mp_uint_t scan_number = 0; scan_number < self->max_key_count; scan_number++) {
        common_hal_digitalio_digitalinout_set_value(self->clock, false);

        // Zero-th data appears on the data pin immediately, without shifting.

        // Loop through all the data pins that share the latch
        mp_uint_t index = 0;

        for (mp_uint_t i = 0; i < self->data_pins->len; i++) {

            // When this data pin has less shiftable bits, ignore it
            if (scan_number >= self->key_counts[i]) {
                continue;
            }

            mp_uint_t key_number = scan_number + index;

            // Get the current state.
            // Get the current state.
            const bool current =
                common_hal_digitalio_digitalinout_get_value(self->data_pins->items[i]) == self->value_when_pressed;

            // Record any transitions.
            if (keypad_debounce((keypad_scanner_obj_t *)self, key_number, current)) {
                keypad_eventqueue_record(self->events, key_number, current, timestamp);
            }

            index += self->key_counts[i];
        }

        // Trigger a shift to get the next bit.
        common_hal_digitalio_digitalinout_set_value(self->clock, true);

    }

    // Start reading the input pins again.
    common_hal_digitalio_digitalinout_set_value(self->latch, !self->value_to_latch);
}
