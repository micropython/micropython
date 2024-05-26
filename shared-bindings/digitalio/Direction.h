// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef enum {
    DIRECTION_INPUT,
    DIRECTION_OUTPUT
} digitalio_direction_t;
typedef struct {
    mp_obj_base_t base;
} digitalio_direction_obj_t;

extern const mp_obj_type_t digitalio_direction_type;

extern const digitalio_direction_obj_t digitalio_direction_input_obj;
extern const digitalio_direction_obj_t digitalio_direction_output_obj;
