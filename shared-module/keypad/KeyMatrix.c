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
#include "shared-bindings/keypad/KeyMatrix.h"
#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"

#define DEBOUNCE_TICKS (20)

static mp_uint_t row_col_to_key_num(keypad_keymatrix_obj_t *self, mp_uint_t row, mp_uint_t col) {
    return row * self->col_digitalinouts->len + col;
}

void common_hal_keypad_keymatrix_construct(keypad_keymatrix_obj_t *self, mp_uint_t num_row_pins, mcu_pin_obj_t *row_pins[], mp_uint_t num_col_pins, mcu_pin_obj_t *col_pins[], bool columns_to_anodes, size_t max_events) {

    mp_obj_t row_dios[num_row_pins];
    for (size_t row = 0; row < num_row_pins; row++) {
        digitalio_digitalinout_obj_t *dio = m_new_obj(digitalio_digitalinout_obj_t);
        dio->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(dio, row_pins[row]);
        common_hal_digitalio_digitalinout_switch_to_input(dio, columns_to_anodes ? PULL_UP : PULL_DOWN);
        row_dios[row] = dio;
    }
    self->row_digitalinouts = mp_obj_new_tuple(num_row_pins, row_dios);

    mp_obj_t col_dios[num_col_pins];
    for (size_t col = 0; col < num_col_pins; col++) {
        digitalio_digitalinout_obj_t *dio = m_new_obj(digitalio_digitalinout_obj_t);
        dio->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(dio, col_pins[col]);
        common_hal_digitalio_digitalinout_switch_to_input(dio, columns_to_anodes ? PULL_UP : PULL_DOWN);
        col_dios[col] = dio;
    }
    self->col_digitalinouts = mp_obj_new_tuple(num_col_pins, col_dios);

    self->currently_pressed = (bool *)gc_alloc(sizeof(bool) * num_row_pins * num_col_pins, false, false);
    self->previously_pressed = (bool *)gc_alloc(sizeof(bool) * num_row_pins * num_col_pins, false, false);

    self->columns_to_anodes = columns_to_anodes;

    keypad_eventqueue_obj_t *events = m_new_obj(keypad_eventqueue_obj_t);
    events->base.type = &keypad_eventqueue_type;
    common_hal_keypad_eventqueue_construct(events, max_events);
    self->events = events;

    // Add self to the list of active keypad scanners.
    keypad_register_scanner((keypad_scanner_obj_t *)self);

    supervisor_enable_tick();
}

void common_hal_keypad_keymatrix_deinit(keypad_keymatrix_obj_t *self) {
    if (common_hal_keypad_keymatrix_deinited(self)) {
        return;
    }

    // Remove self from the list of active keypad scanners first.
    keypad_deregister_scanner((keypad_scanner_obj_t *)self);

    for (size_t row = 0; row < common_hal_keypad_keymatrix_get_num_rows(self); row++) {
        common_hal_digitalio_digitalinout_deinit(self->row_digitalinouts->items[row]);
    }
    self->row_digitalinouts = MP_ROM_NONE;

    for (size_t col = 0; col < common_hal_keypad_keymatrix_get_num_cols(self); col++) {
        common_hal_digitalio_digitalinout_deinit(self->col_digitalinouts->items[col]);
    }
    self->col_digitalinouts = MP_ROM_NONE;
}

bool common_hal_keypad_keymatrix_deinited(keypad_keymatrix_obj_t *self) {
    return self->row_digitalinouts == MP_ROM_NONE;
}

size_t common_hal_keypad_keymatrix_get_num_keys(keypad_keymatrix_obj_t *self) {
    return common_hal_keypad_keymatrix_get_num_rows(self) * common_hal_keypad_keymatrix_get_num_cols(self);
}

size_t common_hal_keypad_keymatrix_get_num_rows(keypad_keymatrix_obj_t *self) {
    return self->row_digitalinouts->len;
}

size_t common_hal_keypad_keymatrix_get_num_cols(keypad_keymatrix_obj_t *self) {
    return self->col_digitalinouts->len;
}

bool common_hal_keypad_keymatrix_pressed(keypad_keymatrix_obj_t *self, mp_uint_t key_num) {
    return self->currently_pressed[key_num];
}

// The length of states has already been validated.
void common_hal_keypad_keymatrix_store_states(keypad_keymatrix_obj_t *self, uint8_t *states) {
    // Read the state atomically.
    supervisor_acquire_lock(&keypad_scanners_linked_list_lock);
    memcpy(states, self->currently_pressed, common_hal_keypad_keymatrix_get_num_keys(self));
    supervisor_release_lock(&keypad_scanners_linked_list_lock);
}

mp_uint_t common_hal_keypad_keymatrix_key_num(keypad_keymatrix_obj_t *self, mp_uint_t row, mp_uint_t col) {
    return row_col_to_key_num(self, row, col);
}

mp_obj_t common_hal_keypad_keymatrix_get_events(keypad_keymatrix_obj_t *self) {
    return MP_OBJ_FROM_PTR(self->events);
}

void keypad_keymatrix_scan(keypad_keymatrix_obj_t *self) {
    uint64_t now = port_get_raw_ticks(NULL);
    if (now - self->last_scan_ticks < DEBOUNCE_TICKS) {
        // Too soon. Wait longer to debounce.
        return;
    }

    self->last_scan_ticks = now;

    // On entry, all pins are set to inputs with a pull-up or pull-down,
    // depending on the diode orientation.
    for (size_t row = 0; row < common_hal_keypad_keymatrix_get_num_rows(self); row++) {
        // Switch this row to an output and set level appropriately
        // Set low if columns_to_anodes is true, else set high.
        common_hal_digitalio_digitalinout_switch_to_output(
            self->row_digitalinouts->items[row], !self->columns_to_anodes, DRIVE_MODE_PUSH_PULL);

        for (size_t col = 0; col < common_hal_keypad_keymatrix_get_num_cols(self); col++) {
            mp_uint_t key_num = row_col_to_key_num(self, row, col);
            const bool previous = self->currently_pressed[key_num];
            self->previously_pressed[key_num] = previous;

            // Get the current state, by reading whether the col got pulled to the row value or not.
            // If low and columns_to_anodes is true, the key is pressed.
            // If high and columns_to_anodes is false, the key is pressed.
            const bool current =
                common_hal_digitalio_digitalinout_get_value(self->col_digitalinouts->items[col]) !=
                self->columns_to_anodes;
            self->currently_pressed[key_num] = current;

            // Record any transitions.
            if (previous != current) {
                keypad_eventqueue_record(self->events, key_num, current);
            }
        }

        // Switch the row back to an input, pulled appropriately
        common_hal_digitalio_digitalinout_switch_to_input(
            self->row_digitalinouts->items[row], self->columns_to_anodes ? PULL_UP : PULL_DOWN);
    }
}
