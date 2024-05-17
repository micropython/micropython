// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Michael Schroeder
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_SUPERVISOR_RUNTIME_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_SUPERVISOR_RUNTIME_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // Stores no state currently.
} super_runtime_obj_t;

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_SUPERVISOR_RUNTIME_H
