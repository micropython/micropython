// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"
#include "shared-bindings/pwmio/PWMOut.h"

typedef struct {
    mp_obj_base_t base;
    __IO PORT_PINCFG_Type *pincfg;
    pwmio_pwmout_obj_t pwmout;
    uint8_t pin;
} pulseio_pulseout_obj_t;

void pulseout_reset(void);
void pulseout_interrupt_handler(uint8_t index);
