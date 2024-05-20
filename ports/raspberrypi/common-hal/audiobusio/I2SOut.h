// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/rp2pio/StateMachine.h"

#include "audio_dma.h"
#include "py/obj.h"

// We don't bit pack because we'll only have two at most. Its better to save code size instead.
typedef struct {
    mp_obj_base_t base;
    rp2pio_statemachine_obj_t state_machine;
    audio_dma_t dma;
    bool left_justified;
    bool playing;
} audiobusio_i2sout_obj_t;

void i2sout_reset(void);
