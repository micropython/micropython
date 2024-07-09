// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "bindings/espulp/Architecture.h"

typedef struct {
    mp_obj_base_t base;
    espulp_architecture_t arch;
    bool inited;
} espulp_ulp_obj_t;
