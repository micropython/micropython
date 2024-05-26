// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/keypad/Event.h"
#include "shared-bindings/keypad/Event.h"

void common_hal_keypad_event_construct(keypad_event_obj_t *self, mp_uint_t key_number, bool pressed, mp_obj_t timestamp) {
    self->key_number = key_number;
    self->pressed = pressed;
    self->timestamp = timestamp;
}

mp_int_t common_hal_keypad_event_get_key_number(keypad_event_obj_t *self) {
    return self->key_number;
}

bool common_hal_keypad_event_get_pressed(keypad_event_obj_t *self) {
    return self->pressed;
}

bool common_hal_keypad_event_get_released(keypad_event_obj_t *self) {
    return !self->pressed;
}

mp_obj_t common_hal_keypad_event_get_timestamp(keypad_event_obj_t *self) {
    return self->timestamp;
}
