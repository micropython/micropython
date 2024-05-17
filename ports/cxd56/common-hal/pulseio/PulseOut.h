// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-bindings/pwmio/PWMOut.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t pwm_num;
    pwmio_pwmout_obj_t pwmout;
} pulseio_pulseout_obj_t;
