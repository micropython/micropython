// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;

    const mcu_pin_obj_t *pin;
    bool idle_state;
    bool paused;
    volatile bool first_edge;

    uint16_t *buffer;
    uint16_t maxlen;

    volatile uint16_t start;
    volatile uint16_t len;
    volatile uint32_t last_overflow;
    volatile uint16_t last_count;
} pulseio_pulsein_obj_t;
