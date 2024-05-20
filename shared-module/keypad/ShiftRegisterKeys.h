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
    digitalio_digitalinout_obj_t *clock;
    digitalio_digitalinout_obj_t *latch;
    mp_obj_tuple_t *data_pins;
    mp_uint_t *key_counts;
    mp_uint_t num_key_counts;
    mp_uint_t max_key_count;
    bool value_when_pressed;
    bool value_to_latch;
} keypad_shiftregisterkeys_obj_t;

void keypad_shiftregisterkeys_scan(keypad_shiftregisterkeys_obj_t *self);
