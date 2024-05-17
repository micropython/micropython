// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#if !CIRCUITPY_AUDIOBUSIO_I2SOUT
#error "audiopwmio requires CIRCUITPY_AUDIOBUSIO_I2SOUT"
#endif

#include "supervisor/background_callback.h"
#include "common-hal/microcontroller/Pin.h"

#include "common-hal/audiobusio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    i2s_t i2s;
    const mcu_pin_obj_t *left_channel, *right_channel;
} audiopwmio_pwmaudioout_obj_t;
