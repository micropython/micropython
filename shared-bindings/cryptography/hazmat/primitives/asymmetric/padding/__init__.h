// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

extern const mp_obj_module_t cryptography_hazmat_primitives_asymmetric_padding_module;

// PKCS1v15 is a plain constant: no parameters, nothing to construct, so
// *_obj is the only thing exposed under that name (the type itself isn't
// reachable, so it can't be called). OAEP takes a real parameter, so it
// stays a constructible type; *_singleton is what its make_new() returns
// after validating arguments.
extern const mp_obj_type_t cryptography_hazmat_primitives_asymmetric_padding_pkcs1v15_type;
extern const mp_obj_base_t cryptography_hazmat_primitives_asymmetric_padding_pkcs1v15_obj;
extern const mp_obj_type_t cryptography_hazmat_primitives_asymmetric_padding_oaep_type;
extern const mp_obj_base_t cryptography_hazmat_primitives_asymmetric_padding_oaep_singleton;
