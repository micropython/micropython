// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

extern const mp_obj_module_t cryptography_hazmat_primitives_asymmetric_ec_module;

// SECP256R1 is a plain constant, like padding.PKCS1v15 and hashes.SHA256: no
// real parameters, so make_new() just validates there are none and hands
// back the one singleton -- it still needs a make_new (unlike those two)
// because real PyCA code always calls `ec.SECP256R1()`.
extern const mp_obj_type_t cryptography_hazmat_primitives_asymmetric_ec_secp256r1_type;
extern const mp_obj_base_t cryptography_hazmat_primitives_asymmetric_ec_secp256r1_obj;

// ECDSA takes a real parameter (the hash/Prehashed algorithm), so it's a
// constructible type, same shape as padding.OAEP.
extern const mp_obj_type_t cryptography_hazmat_primitives_asymmetric_ec_ecdsa_type;

typedef struct {
    mp_obj_base_t base;
    mp_obj_t algorithm;
} cryptography_hazmat_primitives_asymmetric_ec_ecdsa_obj_t;
