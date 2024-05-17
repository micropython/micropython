// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_MODULE_KEYPAD_EVENT_H
#define MICROPY_INCLUDED_SHARED_MODULE_KEYPAD_EVENT_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint16_t key_number;
    bool pressed;
    mp_obj_t timestamp;
} keypad_event_obj_t;


#endif  // MICROPY_INCLUDED_SHARED_MODULE_KEYPAD_EVENT_H
