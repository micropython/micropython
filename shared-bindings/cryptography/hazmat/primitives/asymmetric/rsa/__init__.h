// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-bindings/cryptography/hazmat/primitives/asymmetric/rsa/DigitalSignatureKey.h"

extern const mp_obj_module_t cryptography_hazmat_primitives_asymmetric_rsa_module;

// The module-level loader function (rsa.load_digital_signature_key()).
extern const mp_obj_fun_builtin_fixed_t cryptography_hazmat_primitives_asymmetric_rsa_load_digital_signature_key_obj;
