// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objlist.h"
#include "shared-module/keypad/KeyMatrix.h"

extern const mp_obj_type_t keypad_keymatrix_type;

void common_hal_keypad_keymatrix_construct(keypad_keymatrix_obj_t *self, mp_uint_t num_row_pins, const mcu_pin_obj_t *row_pins[], mp_uint_t num_column_pins, const mcu_pin_obj_t *column_pins[], bool columns_to_anodes, mp_float_t interval, size_t max_events, uint8_t debounce_threshold);

void common_hal_keypad_keymatrix_deinit(keypad_keymatrix_obj_t *self);

void common_hal_keypad_keymatrix_key_number_to_row_column(keypad_keymatrix_obj_t *self, mp_uint_t key_number, mp_uint_t *row, mp_uint_t *column);
mp_uint_t common_hal_keypad_keymatrix_row_column_to_key_number(keypad_keymatrix_obj_t *self, mp_uint_t row, mp_uint_t column);

size_t common_hal_keypad_keymatrix_get_column_count(keypad_keymatrix_obj_t *self);
size_t common_hal_keypad_keymatrix_get_row_count(keypad_keymatrix_obj_t *self);
