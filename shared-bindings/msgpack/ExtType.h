// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Bernhard Boser
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    int32_t code;
    mp_obj_t data;
} mod_msgpack_extype_obj_t;

extern const mp_obj_type_t mod_msgpack_exttype_type;
