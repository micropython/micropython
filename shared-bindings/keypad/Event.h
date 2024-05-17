// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_KEYPAD_EVENT__H
#define MICROPY_INCLUDED_SHARED_BINDINGS_KEYPAD_EVENT__H

#include "py/obj.h"
#include "shared-module/keypad/Event.h"

extern const mp_obj_type_t keypad_event_type;

void common_hal_keypad_event_construct(keypad_event_obj_t *self, mp_uint_t key_number, bool pressed, mp_obj_t timestamp);
mp_int_t common_hal_keypad_event_get_key_number(keypad_event_obj_t *self);
bool common_hal_keypad_event_get_pressed(keypad_event_obj_t *self);
bool common_hal_keypad_event_get_released(keypad_event_obj_t *self);
mp_obj_t common_hal_keypad_event_get_timestamp(keypad_event_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_KEYPAD_EVENT__H
