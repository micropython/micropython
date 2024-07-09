// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objproperty.h"
#include "shared-module/keypad/__init__.h"

bool common_hal_keypad_deinited(void *self);
void common_hal_keypad_generic_reset(void *self);
size_t common_hal_keypad_generic_get_key_count(void *self);
mp_obj_t common_hal_keypad_generic_get_events(void *self);

MP_DECLARE_CONST_FUN_OBJ_1(keypad_generic_reset_obj);

extern const mp_obj_property_t keypad_generic_events_obj;
extern const mp_obj_property_t keypad_generic_key_count_obj;
