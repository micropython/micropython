// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;

    uint8_t pin;
    bool idle_state;
    bool paused;
    volatile bool first_edge;

    uint16_t *buffer;
    uint16_t maxlen;

    volatile uint16_t start;
    volatile uint16_t len;
    volatile size_t last_overflow;
    volatile size_t last_count;
} pulseio_pulsein_obj_t;
