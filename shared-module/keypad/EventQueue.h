// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/ringbuf.h"

typedef struct _keypad_eventqueue_obj_t keypad_eventqueue_obj_t;

struct _keypad_eventqueue_obj_t {
    mp_obj_base_t base;
    ringbuf_t encoded_events;
    bool overflowed;
    void (*event_handler)(keypad_eventqueue_obj_t *);
};

bool keypad_eventqueue_record(keypad_eventqueue_obj_t *self, mp_uint_t key_number, bool pressed, mp_obj_t timestamp);
