// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/enum.h"

typedef enum {
    FSM,
    RISCV
} espulp_architecture_t;

extern const mp_obj_type_t espulp_architecture_type;
extern const cp_enum_obj_t architecture_FSM_obj;
