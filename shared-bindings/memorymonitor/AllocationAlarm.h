// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_MEMORYMONITOR_ALLOCATIONALARM_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_MEMORYMONITOR_ALLOCATIONALARM_H

#include "shared-module/memorymonitor/AllocationAlarm.h"

extern const mp_obj_type_t memorymonitor_allocationalarm_type;

void common_hal_memorymonitor_allocationalarm_construct(memorymonitor_allocationalarm_obj_t *self, size_t minimum_block_count);
void common_hal_memorymonitor_allocationalarm_pause(memorymonitor_allocationalarm_obj_t *self);
void common_hal_memorymonitor_allocationalarm_resume(memorymonitor_allocationalarm_obj_t *self);
void common_hal_memorymonitor_allocationalarm_set_ignore(memorymonitor_allocationalarm_obj_t *self, mp_int_t count);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_MEMORYMONITOR_ALLOCATIONALARM_H
