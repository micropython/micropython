// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_MEMORYMAP_ADDRESSRANGE_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_MEMORYMAP_ADDRESSRANGE_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t *start_address;
    size_t len;
} memorymap_addressrange_obj_t;

#endif // MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_MEMORYMAP_ADDRESSRANGE_H
