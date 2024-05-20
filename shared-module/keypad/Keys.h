// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objtuple.h"

#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-module/keypad/__init__.h"
#include "shared-module/keypad/EventQueue.h"

typedef struct {
    KEYPAD_SCANNER_COMMON_FIELDS;
    mp_obj_tuple_t *digitalinouts;
    bool value_when_pressed;
} keypad_keys_obj_t;

void keypad_keys_scan(keypad_keys_obj_t *self);
