// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objlist.h"
#include "shared-module/keypad/Keys.h"

extern const mp_obj_type_t keypad_keys_type;

void common_hal_keypad_keys_construct(keypad_keys_obj_t *self, mp_uint_t num_pins, const mcu_pin_obj_t *pins[], bool value_when_pressed,  bool pull, mp_float_t interval, size_t max_events, uint8_t debounce_threshold);

void common_hal_keypad_keys_deinit(keypad_keys_obj_t *self);
