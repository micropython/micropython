// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

extern const mp_obj_module_t cryptography_exceptions_module;

extern const mp_obj_type_t mp_type_cryptography_InvalidSignature;

MP_NORETURN void mp_raise_cryptography_InvalidSignature(mp_rom_error_text_t fmt, ...);
