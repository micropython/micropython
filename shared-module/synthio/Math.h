// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once
#include "shared-module/synthio/block.h"

typedef struct synthio_math_obj {
    synthio_block_base_t base;
    synthio_block_slot_t inputs[3];
    synthio_math_operation_t operation;
} synthio_math_obj_t;
