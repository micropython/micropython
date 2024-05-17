// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Some boards don't implement I2SOut, so suppress any routines from here.
#if CIRCUITPY_AUDIOBUSIO_I2SOUT
#include "supervisor/background_callback.h"
#include "common-hal/microcontroller/Pin.h"

#include "common-hal/audiobusio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    i2s_t i2s;
    const mcu_pin_obj_t *bit_clock;
    const mcu_pin_obj_t *word_select;
    const mcu_pin_obj_t *data;
    const mcu_pin_obj_t *mclk;
    uint8_t instance;
} audiobusio_i2sout_obj_t;

#endif
