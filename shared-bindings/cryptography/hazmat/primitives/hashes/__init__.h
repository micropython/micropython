// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

extern const mp_obj_module_t cryptography_hazmat_primitives_hashes_module;

// SHA256 is a plain constant: no parameters, nothing to construct, so
// *_obj is the only thing exposed under that name (the type itself isn't
// reachable, so it can't be called).
extern const mp_obj_type_t cryptography_hazmat_primitives_hashes_sha256_type;
extern const mp_obj_base_t cryptography_hazmat_primitives_hashes_sha256_obj;
