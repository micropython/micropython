// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_MEMORYMONITOR___INIT___H
#define MICROPY_INCLUDED_MEMORYMONITOR___INIT___H

#include <stddef.h>

void memorymonitor_track_allocation(size_t block_count);
void memorymonitor_reset(void);

#endif  // MICROPY_INCLUDED_MEMORYMONITOR___INIT___H
