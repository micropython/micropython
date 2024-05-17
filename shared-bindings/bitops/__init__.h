// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdlib.h>

void common_hal_bitops_bit_transpose(uint8_t *result, const uint8_t *src, size_t inlen, size_t num_strands);
