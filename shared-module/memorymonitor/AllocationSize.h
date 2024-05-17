// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_MODULE_MEMORYMONITOR_ALLOCATIONSIZE_H
#define MICROPY_INCLUDED_SHARED_MODULE_MEMORYMONITOR_ALLOCATIONSIZE_H

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

typedef struct _memorymonitor_allocationsize_obj_t memorymonitor_allocationsize_obj_t;

#define ALLOCATION_SIZE_BUCKETS 16

typedef struct _memorymonitor_allocationsize_obj_t {
    mp_obj_base_t base;
    uint16_t buckets[ALLOCATION_SIZE_BUCKETS];
    // Store the location that points to us so we can remove ourselves.
    memorymonitor_allocationsize_obj_t **previous;
    memorymonitor_allocationsize_obj_t *next;
    bool paused;
} memorymonitor_allocationsize_obj_t;

void memorymonitor_allocationsizes_track_allocation(size_t block_count);
void memorymonitor_allocationsizes_reset(void);

#endif // MICROPY_INCLUDED_SHARED_MODULE_MEMORYMONITOR_ALLOCATIONSIZE_H
