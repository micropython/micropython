// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "bindings/rp2pio/StateMachine.h"

#include "extmod/vfs_fat.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint32_t sample_rate;
    uint8_t serializer;
    uint8_t clock_unit;
    uint8_t bytes_per_sample;
    uint8_t bit_depth;
    rp2pio_statemachine_obj_t state_machine;
} audiobusio_pdmin_obj_t;

void pdmin_reset(void);

void pdmin_background(void);
