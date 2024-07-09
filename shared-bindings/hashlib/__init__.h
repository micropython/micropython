// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "shared-bindings/hashlib/Hash.h"

bool common_hal_hashlib_new(hashlib_hash_obj_t *self, const char *algorithm);
