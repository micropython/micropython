// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

#include "py/enum.h"

#include "bindings/espulp/Architecture.h"

MAKE_ENUM_VALUE(espulp_architecture_type, architecture, FSM, FSM);
MAKE_ENUM_VALUE(espulp_architecture_type, architecture, RISCV, RISCV);

//| class Architecture:
//|     """The ULP architectures available."""
//|
//|     FSM: Architecture
//|     """The ULP Finite State Machine."""
//|
//|     RISCV: Architecture
//|     """The ULP RISC-V Coprocessor."""
//|
MAKE_ENUM_MAP(espulp_architecture) {
    MAKE_ENUM_MAP_ENTRY(architecture, FSM),
    MAKE_ENUM_MAP_ENTRY(architecture, RISCV),
};
static MP_DEFINE_CONST_DICT(espulp_architecture_locals_dict, espulp_architecture_locals_table);

MAKE_PRINTER(espulp, espulp_architecture);

MAKE_ENUM_TYPE(espulp, Architecture, espulp_architecture);
