// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objlist.h"
#include "shared-module/keypad/ShiftRegisterKeys.h"

extern const mp_obj_type_t keypad_shiftregisterkeys_type;

void common_hal_keypad_shiftregisterkeys_construct(keypad_shiftregisterkeys_obj_t *self, const mcu_pin_obj_t *clock_pin, mp_uint_t num_data_pins, const mcu_pin_obj_t *data_pins[], const mcu_pin_obj_t *latch_pin, bool value_to_latch, size_t num_key_count, size_t key_counts[], bool value_when_pressed, mp_float_t interval, size_t max_events, uint8_t debounce_threshold);

void common_hal_keypad_shiftregisterkeys_deinit(keypad_shiftregisterkeys_obj_t *self);
