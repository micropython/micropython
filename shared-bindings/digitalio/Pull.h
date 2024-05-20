// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef enum _digitalio_pull_t {
    PULL_NONE,
    PULL_UP,
    PULL_DOWN
} digitalio_pull_t;

extern const mp_obj_type_t digitalio_pull_type;

typedef struct {
    mp_obj_base_t base;
} digitalio_pull_obj_t;
extern const digitalio_pull_obj_t digitalio_pull_up_obj;
extern const digitalio_pull_obj_t digitalio_pull_down_obj;

digitalio_pull_t validate_pull(mp_rom_obj_t obj, qstr arg_name);
