/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 MicroDev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/enum.h"

#include "bindings/espulp/ULPArch.h"

MAKE_ENUM_VALUE(espulp_ulparch_type, ulparch, FSM, FSM);
MAKE_ENUM_VALUE(espulp_ulparch_type, ulparch, RISCV, RISCV);

//| class ULPArch:
//|     """The ULP architectures available."""
//|
//|     FSM: ULPArch
//|     """The ULP Finite State Machine."""
//|
//|     RISCV: ULPArch
//|     """The ULP RISC-V Coprocessor."""
//|
MAKE_ENUM_MAP(espulp_ulparch) {
    MAKE_ENUM_MAP_ENTRY(ulparch, FSM),
    MAKE_ENUM_MAP_ENTRY(ulparch, RISCV),
};
STATIC MP_DEFINE_CONST_DICT(espulp_ulparch_locals_dict, espulp_ulparch_locals_table);

MAKE_PRINTER(espulp, espulp_ulparch);

MAKE_ENUM_TYPE(espulp, ULPArch, espulp_ulparch);
