// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/enum.h"
#include "py/obj.h"

typedef enum _countio_edge_t {
    EDGE_RISE,
    EDGE_FALL,
    EDGE_RISE_AND_FALL,
} countio_edge_t;

extern const mp_obj_type_t countio_edge_type;
extern const cp_enum_obj_t edge_FALL_obj;

countio_edge_t validate_edge(mp_obj_t obj, qstr arg_name);
