// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pwmio/PWMOut.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    pwmio_pwmout_obj_t pwmout;
} pulseio_pulseout_obj_t;
