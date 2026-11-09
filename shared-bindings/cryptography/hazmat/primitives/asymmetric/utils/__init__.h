// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

extern const mp_obj_module_t cryptography_hazmat_primitives_asymmetric_utils_module;

// Prehashed takes a real parameter (the hash algorithm), same shape as
// padding.OAEP / ec.ECDSA.
extern const mp_obj_type_t cryptography_hazmat_primitives_asymmetric_utils_prehashed_type;

typedef struct {
    mp_obj_base_t base;
    mp_obj_t algorithm;
} cryptography_hazmat_primitives_asymmetric_utils_prehashed_obj_t;
