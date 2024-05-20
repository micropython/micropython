// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/memorymonitor/AllocationSize.h"

extern const mp_obj_type_t memorymonitor_allocationsize_type;

extern void common_hal_memorymonitor_allocationsize_construct(memorymonitor_allocationsize_obj_t *self);
extern void common_hal_memorymonitor_allocationsize_pause(memorymonitor_allocationsize_obj_t *self);
extern void common_hal_memorymonitor_allocationsize_resume(memorymonitor_allocationsize_obj_t *self);
extern void common_hal_memorymonitor_allocationsize_clear(memorymonitor_allocationsize_obj_t *self);
extern size_t common_hal_memorymonitor_allocationsize_get_bytes_per_block(memorymonitor_allocationsize_obj_t *self);
extern uint16_t common_hal_memorymonitor_allocationsize_get_len(memorymonitor_allocationsize_obj_t *self);
extern uint16_t common_hal_memorymonitor_allocationsize_get_item(memorymonitor_allocationsize_obj_t *self, int16_t index);
