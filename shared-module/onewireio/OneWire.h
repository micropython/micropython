// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/digitalio/DigitalInOut.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    digitalio_digitalinout_obj_t pin;
} onewireio_onewire_obj_t;
