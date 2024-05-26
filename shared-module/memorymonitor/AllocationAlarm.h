// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

typedef struct _memorymonitor_allocationalarm_obj_t memorymonitor_allocationalarm_obj_t;

#define ALLOCATION_SIZE_BUCKETS 16

typedef struct _memorymonitor_allocationalarm_obj_t {
    mp_obj_base_t base;
    size_t minimum_block_count;
    mp_int_t count;
    // Store the location that points to us so we can remove ourselves.
    memorymonitor_allocationalarm_obj_t **previous;
    memorymonitor_allocationalarm_obj_t *next;
} memorymonitor_allocationalarm_obj_t;

void memorymonitor_allocationalarms_allocation(size_t block_count);
void memorymonitor_allocationalarms_reset(void);
