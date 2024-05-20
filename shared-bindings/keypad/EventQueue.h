// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/keypad/Event.h"
#include "shared-module/keypad/EventQueue.h"

extern const mp_obj_type_t keypad_eventqueue_type;

void common_hal_keypad_eventqueue_construct(keypad_eventqueue_obj_t *self, size_t max_events);

void common_hal_keypad_eventqueue_clear(keypad_eventqueue_obj_t *self);
size_t common_hal_keypad_eventqueue_get_length(keypad_eventqueue_obj_t *self);
mp_obj_t common_hal_keypad_eventqueue_get(keypad_eventqueue_obj_t *self);
bool common_hal_keypad_eventqueue_get_into(keypad_eventqueue_obj_t *self, keypad_event_obj_t *event);

bool common_hal_keypad_eventqueue_get_overflowed(keypad_eventqueue_obj_t *self);
void common_hal_keypad_eventqueue_set_overflowed(keypad_eventqueue_obj_t *self, bool overflowed);

void common_hal_keypad_eventqueue_set_event_handler(keypad_eventqueue_obj_t *self, void (*event_handler)(keypad_eventqueue_obj_t *));
