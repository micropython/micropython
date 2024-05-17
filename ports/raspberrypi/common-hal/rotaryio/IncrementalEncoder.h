// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/rp2pio/StateMachine.h"
#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    rp2pio_statemachine_obj_t state_machine;
    uint8_t state;        // <old A><old B>
    int8_t sub_count; // count intermediate transitions between detents
    int8_t divisor; // Number of quadrature edges required per count
    bool swapped;         // Did the pins need to be swapped to be sequential?
    mp_int_t position;
} rotaryio_incrementalencoder_obj_t;
