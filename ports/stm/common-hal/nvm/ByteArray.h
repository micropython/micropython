// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// STM flash is saved in sectors (not pages), at a minimum size of 16k.
// To limit the RAM usage during writing, we want to set a smaller
// maximum value.
#ifndef NVM_BYTEARRAY_BUFFER_SIZE
#define NVM_BYTEARRAY_BUFFER_SIZE 512
#endif

typedef struct {
    mp_obj_base_t base;
    uint8_t *start_address;
    uint32_t len;
} nvm_bytearray_obj_t;
