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
    uint8_t channel;
    uint8_t pin;
    uint16_t *buffer;
    uint16_t maxlen;
    bool idle_state;
    volatile uint16_t start;
    volatile uint16_t len;
    volatile bool first_edge;
    volatile uint32_t last_overflow;
    volatile uint16_t last_count;
    volatile bool errored_too_fast;
} pulseio_pulsein_obj_t;

void pulsein_interrupt_handler(uint8_t channel);
void pulsein_timer_interrupt_handler(uint8_t index);
