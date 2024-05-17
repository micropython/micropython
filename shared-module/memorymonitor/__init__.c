// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#include "shared-module/memorymonitor/__init__.h"
#include "shared-module/memorymonitor/AllocationAlarm.h"
#include "shared-module/memorymonitor/AllocationSize.h"

void memorymonitor_track_allocation(size_t block_count) {
    memorymonitor_allocationalarms_allocation(block_count);
    memorymonitor_allocationsizes_track_allocation(block_count);
}

void memorymonitor_reset(void) {
    memorymonitor_allocationalarms_reset();
    memorymonitor_allocationsizes_reset();
}
