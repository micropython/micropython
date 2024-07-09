// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

// Native touchio only exists for SAMD21
#ifdef SAMD21

#include "common-hal/microcontroller/Pin.h"

#include "samd21_ptc_component.h"
#include "adafruit_ptc.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    struct adafruit_ptc_config config;
    uint16_t threshold;
} touchio_touchin_obj_t;

void touchin_reset(void);

#endif // SAMD21
