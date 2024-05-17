// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/digitalio/Pull.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    bool output;
    bool open_drain;
    digitalio_pull_t pull;
} digitalio_digitalinout_obj_t;

void pin_config(const mcu_pin_obj_t *pin, bool open_drain, digitalio_pull_t pull);
