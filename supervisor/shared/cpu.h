// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SUPERVISOR_SHARED_CPU_H
#define MICROPY_INCLUDED_SUPERVISOR_SHARED_CPU_H

#include <stdbool.h>
#include <stddef.h>

// True when we're in an interrupt handler.
bool cpu_interrupt_active(void);

#endif  // MICROPY_INCLUDED_SUPERVISOR_SHARED_CPU_H
