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
#include "shared-bindings/keypad/KeyMatrix.h"
#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"

static void keymatrix_scan_now(void *self_in, mp_obj_t timestamp);
static size_t keymatrix_get_key_count(void *self_in);

static keypad_scanner_funcs_t keymatrix_funcs = {
    .scan_now = keymatrix_scan_now,
    .get_key_count = keymatrix_get_key_count,
};

static mp_uint_t row_column_to_key_number(keypad_keymatrix_obj_t *self, mp_uint_t row, mp_uint_t column) {
    return row * self->column_digitalinouts->len + column;
}

void common_hal_keypad_keymatrix_construct(keypad_keymatrix_obj_t *self, mp_uint_t num_row_pins, const mcu_pin_obj_t *row_pins[], mp_uint_t num_column_pins, const mcu_pin_obj_t *column_pins[], bool columns_to_anodes, mp_float_t interval, size_t max_events, uint8_t debounce_threshold) {

    mp_obj_t row_dios[num_row_pins];
    for (size_t row = 0; row < num_row_pins; row++) {
        digitalio_digitalinout_obj_t *dio =
            mp_obj_malloc(digitalio_digitalinout_obj_t, &digitalio_digitalinout_type);
        common_hal_digitalio_digitalinout_construct(dio, row_pins[row]);
        common_hal_digitalio_digitalinout_switch_to_input(dio, columns_to_anodes ? PULL_UP : PULL_DOWN);
        row_dios[row] = dio;
    }
    self->row_digitalinouts = mp_obj_new_tuple(num_row_pins, row_dios);

    mp_obj_t column_dios[num_column_pins];
    for (size_t column = 0; column < num_column_pins; column++) {
        digitalio_digitalinout_obj_t *dio =
            mp_obj_malloc(digitalio_digitalinout_obj_t, &digitalio_digitalinout_type);
        dio->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(dio, column_pins[column]);
        common_hal_digitalio_digitalinout_switch_to_input(dio, columns_to_anodes ? PULL_UP : PULL_DOWN);
        column_dios[column] = dio;
    }
    self->column_digitalinouts = mp_obj_new_tuple(num_column_pins, column_dios);

    self->columns_to_anodes = columns_to_anodes;
    self->funcs = &keymatrix_funcs;

    keypad_construct_common((keypad_scanner_obj_t *)self, interval, max_events, debounce_threshold);
}

void common_hal_keypad_keymatrix_deinit(keypad_keymatrix_obj_t *self) {
    if (common_hal_keypad_deinited(self)) {
        return;
    }

    // Remove self from the list of active keypad scanners first.
    keypad_deregister_scanner((keypad_scanner_obj_t *)self);

    for (size_t row = 0; row < common_hal_keypad_keymatrix_get_row_count(self); row++) {
        common_hal_digitalio_digitalinout_deinit(self->row_digitalinouts->items[row]);
    }
    self->row_digitalinouts = MP_ROM_NONE;

    for (size_t column = 0; column < common_hal_keypad_keymatrix_get_column_count(self); column++) {
        common_hal_digitalio_digitalinout_deinit(self->column_digitalinouts->items[column]);
    }
    self->column_digitalinouts = MP_ROM_NONE;
    common_hal_keypad_deinit_core(self);
}

size_t common_hal_keypad_keymatrix_get_row_count(keypad_keymatrix_obj_t *self) {
    return self->row_digitalinouts->len;
}

size_t common_hal_keypad_keymatrix_get_column_count(keypad_keymatrix_obj_t *self) {
    return self->column_digitalinouts->len;
}

mp_uint_t common_hal_keypad_keymatrix_row_column_to_key_number(keypad_keymatrix_obj_t *self, mp_uint_t row, mp_uint_t column) {
    return row_column_to_key_number(self, row, column);
}

void common_hal_keypad_keymatrix_key_number_to_row_column(keypad_keymatrix_obj_t *self, mp_uint_t key_number, mp_uint_t *row, mp_uint_t *column) {
    const size_t num_columns = common_hal_keypad_keymatrix_get_column_count(self);
    *row = key_number / num_columns;
    *column = key_number % num_columns;
}

static size_t keymatrix_get_key_count(void *self_in) {
    keypad_keymatrix_obj_t *self = self_in;
    return common_hal_keypad_keymatrix_get_column_count(self) * common_hal_keypad_keymatrix_get_row_count(self);
}

static void keymatrix_scan_now(void *self_in, mp_obj_t timestamp) {
    keypad_keymatrix_obj_t *self = self_in;

    // On entry, all pins are set to inputs with a pull-up or pull-down,
    // depending on the diode orientation.
    for (size_t row = 0; row < common_hal_keypad_keymatrix_get_row_count(self); row++) {
        // Switch this row to an output and set level appropriately
        // Set low if columns_to_anodes is true, else set high.
        digitalio_digitalinout_obj_t *row_dio = self->row_digitalinouts->items[row];
        common_hal_digitalio_digitalinout_switch_to_output(
            row_dio, !self->columns_to_anodes, DRIVE_MODE_PUSH_PULL);

        for (size_t column = 0; column < common_hal_keypad_keymatrix_get_column_count(self); column++) {
            mp_uint_t key_number = row_column_to_key_number(self, row, column);

            // Get the current state, by reading whether the column got pulled to the row value or not.
            // If low and columns_to_anodes is true, the key is pressed.
            // If high and columns_to_anodes is false, the key is pressed.
            const bool current =
                common_hal_digitalio_digitalinout_get_value(self->column_digitalinouts->items[column]) !=
                self->columns_to_anodes;

            // Record any transitions.
            if (keypad_debounce((keypad_scanner_obj_t *)self, key_number, current)) {
                keypad_eventqueue_record(self->events, key_number, current, timestamp);
            }
        }

        // Done with this row. Set its pin to its resting pull value briefly to shorten the time it takes
        // to switch values. Just switching to an input with a (relatively weak) pullup/pulldown
        // causes a slight delay in the output changing, which can cause false readings.
        common_hal_digitalio_digitalinout_set_value(row_dio, self->columns_to_anodes);
        // Switch the row back to an input, pulled appropriately
        common_hal_digitalio_digitalinout_switch_to_input(
            row_dio, self->columns_to_anodes ? PULL_UP : PULL_DOWN);
    }
}
