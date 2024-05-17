// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_HASHLIB___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_HASHLIB___INIT___H

#include <stdbool.h>

#include "shared-bindings/hashlib/Hash.h"

bool common_hal_hashlib_new(hashlib_hash_obj_t *self, const char *algorithm);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_HASHLIB___INIT___H
