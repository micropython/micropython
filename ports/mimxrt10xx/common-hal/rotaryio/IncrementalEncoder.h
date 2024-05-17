// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin_a, *pin_b;
    uint8_t state;        // <old A><old B>
    int8_t sub_count; // count intermediate transitions between detents
    int8_t divisor; // Number of quadrature edges required per count
    mp_int_t position;
} rotaryio_incrementalencoder_obj_t;


void incrementalencoder_interrupt_handler(uint8_t channel);
