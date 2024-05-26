// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George
//
// SPDX-License-Identifier: MIT

#pragma once

// select nan-boxing object model
#define MICROPY_OBJ_REPR (MICROPY_OBJ_REPR_D)

// native emitters don't work with nan-boxing
#define MICROPY_EMIT_X86 (0)
#define MICROPY_EMIT_X64 (0)
#define MICROPY_EMIT_THUMB (0)
#define MICROPY_EMIT_ARM (0)

#include <stdint.h>

typedef int64_t mp_int_t;
typedef uint64_t mp_uint_t;
#define UINT_FMT "%llu"
#define INT_FMT "%lld"

#include <mpconfigport.h>
