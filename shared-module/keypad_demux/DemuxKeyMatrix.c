// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 CDarius
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/keypad_demux/DemuxKeyMatrix.h"
#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"

static void demuxkeymatrix_scan_now(void *self_in, mp_obj_t timestamp);
static size_t demuxkeymatrix_get_key_count(void *self_in);

static keypad_scanner_funcs_t keymatrix_funcs = {
    .scan_now = demuxkeymatrix_scan_now,
    .get_key_count = demuxkeymatrix_get_key_count,
};

static mp_uint_t row_column_to_key_number(keypad_demux_demuxkeymatrix_obj_t *self, mp_uint_t row, mp_uint_t column) {
    return row * self->column_digitalinouts->len + column;
}

void common_hal_keypad_demux_demuxkeymatrix_construct(keypad_demux_demuxkeymatrix_obj_t *self, mp_uint_t num_row_addr_pins, const mcu_pin_obj_t *row_addr_pins[], mp_uint_t num_column_pins, const mcu_pin_obj_t *column_pins[], mp_float_t interval, size_t max_events, uint8_t debounce_threshold) {

    mp_obj_t row_addr_dios[num_row_addr_pins];
    for (size_t row = 0; row < num_row_addr_pins; row++) {
        digitalio_digitalinout_obj_t *dio =
            mp_obj_malloc(digitalio_digitalinout_obj_t, &digitalio_digitalinout_type);
        common_hal_digitalio_digitalinout_construct(dio, row_addr_pins[row]);
        common_hal_digitalio_digitalinout_switch_to_output(dio, false, DRIVE_MODE_PUSH_PULL);
        row_addr_dios[row] = dio;
    }
    self->row_addr_digitalinouts = mp_obj_new_tuple(num_row_addr_pins, row_addr_dios);

    mp_obj_t column_dios[num_column_pins];
    for (size_t column = 0; column < num_column_pins; column++) {
        digitalio_digitalinout_obj_t *dio =
            mp_obj_malloc(digitalio_digitalinout_obj_t, &digitalio_digitalinout_type);
        dio->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(dio, column_pins[column]);
        common_hal_digitalio_digitalinout_switch_to_input(dio, PULL_UP);
        column_dios[column] = dio;
    }
    self->column_digitalinouts = mp_obj_new_tuple(num_column_pins, column_dios);

    self->funcs = &keymatrix_funcs;

    keypad_construct_common((keypad_scanner_obj_t *)self, interval, max_events, debounce_threshold);
}

void common_hal_keypad_demux_demuxkeymatrix_deinit(keypad_demux_demuxkeymatrix_obj_t *self) {
    if (common_hal_keypad_deinited(self)) {
        return;
    }

    // Remove self from the list of active keypad scanners first.
    keypad_deregister_scanner((keypad_scanner_obj_t *)self);

    for (size_t row_addr = 0; row_addr < self->row_addr_digitalinouts->len; row_addr++) {
        common_hal_digitalio_digitalinout_deinit(self->row_addr_digitalinouts->items[row_addr]);
    }
    self->row_addr_digitalinouts = MP_ROM_NONE;

    for (size_t column = 0; column < self->column_digitalinouts->len; column++) {
        common_hal_digitalio_digitalinout_deinit(self->column_digitalinouts->items[column]);
    }
    self->column_digitalinouts = MP_ROM_NONE;
    common_hal_keypad_deinit_core(self);
}

size_t common_hal_keypad_demux_demuxkeymatrix_get_row_count(keypad_demux_demuxkeymatrix_obj_t *self) {
    return 1 << self->row_addr_digitalinouts->len;
}

size_t common_hal_keypad_demux_demuxkeymatrix_get_column_count(keypad_demux_demuxkeymatrix_obj_t *self) {
    return self->column_digitalinouts->len;
}

mp_uint_t common_hal_keypad_demux_demuxkeymatrix_row_column_to_key_number(keypad_demux_demuxkeymatrix_obj_t *self, mp_uint_t row, mp_uint_t column) {
    return row_column_to_key_number(self, row, column);
}

void common_hal_keypad_demux_demuxkeymatrix_key_number_to_row_column(keypad_demux_demuxkeymatrix_obj_t *self, mp_uint_t key_number, mp_uint_t *row, mp_uint_t *column) {
    const size_t num_columns = common_hal_keypad_demux_demuxkeymatrix_get_column_count(self);
    *row = key_number / num_columns;
    *column = key_number % num_columns;
}

static size_t demuxkeymatrix_get_key_count(void *self_in) {
    keypad_demux_demuxkeymatrix_obj_t *self = self_in;
    return common_hal_keypad_demux_demuxkeymatrix_get_column_count(self) * common_hal_keypad_demux_demuxkeymatrix_get_row_count(self);
}

static void demuxkeymatrix_scan_now(void *self_in, mp_obj_t timestamp) {
    keypad_demux_demuxkeymatrix_obj_t *self = self_in;

    for (size_t row = 0; row < common_hal_keypad_demux_demuxkeymatrix_get_row_count(self); row++) {
        // Set the row address on demultiplexer
        size_t mask = 0b00000001;
        for (size_t row_addr_pin = 0; row_addr_pin < self->row_addr_digitalinouts->len; row_addr_pin++) {
            digitalio_digitalinout_obj_t *dio = self->row_addr_digitalinouts->items[row_addr_pin];
            common_hal_digitalio_digitalinout_set_value(dio, (mask & row) != 0);
            mask = mask << 1;
        }

        for (size_t column = 0; column < common_hal_keypad_demux_demuxkeymatrix_get_column_count(self); column++) {
            mp_uint_t key_number = row_column_to_key_number(self, row, column);

            // Get the current state, by reading whether the column got pulled to the row value or not.
            // If low, the key is pressed.
            const bool current = !common_hal_digitalio_digitalinout_get_value(self->column_digitalinouts->items[column]);

            // Record any transitions.
            if (keypad_debounce((keypad_scanner_obj_t *)self, key_number, current)) {
                keypad_eventqueue_record(self->events, key_number, current, timestamp);
            }
        }
    }
}

void demuxkeymatrix_never_reset(keypad_demux_demuxkeymatrix_obj_t *self) {
    keypad_never_reset((keypad_scanner_obj_t *)self);
    for (size_t row_addr = 0; row_addr < self->row_addr_digitalinouts->len; row_addr++) {
        common_hal_digitalio_digitalinout_never_reset(self->row_addr_digitalinouts->items[row_addr]);
    }
    for (size_t column = 0; column < self->column_digitalinouts->len; column++) {
        common_hal_digitalio_digitalinout_never_reset(self->column_digitalinouts->items[column]);
    }
}
