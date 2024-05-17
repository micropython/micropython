// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_NVM_BYTEARRAY_H
#define MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_NVM_BYTEARRAY_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t *start_address;
    uint32_t len;
} nvm_bytearray_obj_t;

#endif // MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_NVM_BYTEARRAY_H
