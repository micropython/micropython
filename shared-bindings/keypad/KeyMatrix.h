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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_KEYPAD_KEYMATRIX_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_KEYPAD_KEYMATRIX_H

#include "py/objlist.h"
#include "shared-module/keypad/KeyMatrix.h"

extern const mp_obj_type_t keypad_keymatrix_type;

void common_hal_keypad_keymatrix_construct(keypad_keymatrix_obj_t *self, mp_uint_t num_row_pins, mcu_pin_obj_t *row_pins[], mp_uint_t num_column_pins, mcu_pin_obj_t *column_pins[], bool columns_to_anodes, mp_float_t interval, size_t max_events);

void common_hal_keypad_keymatrix_deinit(keypad_keymatrix_obj_t *self);
bool common_hal_keypad_keymatrix_deinited(keypad_keymatrix_obj_t *self);

void common_hal_keypad_keymatrix_key_number_to_row_column(keypad_keymatrix_obj_t *self, mp_uint_t key_number, mp_uint_t *row, mp_uint_t *column);
mp_uint_t common_hal_keypad_keymatrix_row_column_to_key_number(keypad_keymatrix_obj_t *self, mp_uint_t row, mp_uint_t column);

size_t common_hal_keypad_keymatrix_get_key_count(keypad_keymatrix_obj_t *self);
size_t common_hal_keypad_keymatrix_get_column_count(keypad_keymatrix_obj_t *self);
size_t common_hal_keypad_keymatrix_get_row_count(keypad_keymatrix_obj_t *self);

mp_obj_t common_hal_keypad_keymatrix_get_events(keypad_keymatrix_obj_t *self);
void common_hal_keypad_keymatrix_reset(keypad_keymatrix_obj_t *self);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_KEYPAD_KEYMATRIX_H
