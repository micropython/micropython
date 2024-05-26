// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
// SPDX-FileCopyrightText: Copyright (c) 2023 Bob Abeles
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// depending on the section memory type, different access methods and rules apply
typedef enum { SRAM, ROM, XIP, IO } memorymap_rp2_section_t;

typedef struct {
    mp_obj_base_t base;
    uint8_t *start_address;
    size_t len;
    memorymap_rp2_section_t type;
} memorymap_addressrange_obj_t;

typedef struct {
    uint8_t *start_address;
    size_t len;
    memorymap_rp2_section_t type;
} addressmap_rp2_range_t;
