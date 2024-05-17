// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Sean Cross for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_WATCHDOG_WATCHDOGMODE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_WATCHDOG_WATCHDOGMODE_H

#include "py/enum.h"

typedef enum {
    WATCHDOGMODE_NONE,
    WATCHDOGMODE_RAISE,
    WATCHDOGMODE_RESET,
} watchdog_watchdogmode_t;

extern const mp_obj_type_t watchdog_watchdogmode_type;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_WATCHDOG_WATCHDOGMODE_H
