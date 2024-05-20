// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Michael Schroeder
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t tc_index;
    uint8_t pin;
    uint8_t channel;
    uint8_t event_channel;
    uint32_t frequency;
    volatile uint64_t last_ns;
    float factor;
    uint32_t capture_period;
    uint8_t TC_IRQ;
    volatile bool errored_too_fast;
} frequencyio_frequencyin_obj_t;

void frequencyin_interrupt_handler(uint8_t index);
void frequencyin_reset(void);
