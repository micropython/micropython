// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

#include "py/enum.h"

#include "shared-bindings/watchdog/WatchDogMode.h"

MAKE_ENUM_VALUE(watchdog_watchdogmode_type, watchdogmode, RAISE, WATCHDOGMODE_RAISE);
MAKE_ENUM_VALUE(watchdog_watchdogmode_type, watchdogmode, RESET, WATCHDOGMODE_RESET);

//| class WatchDogMode:
//|     """Run state of the watchdog timer."""
//|
//|     RAISE: WatchDogMode
//|     """Raise an exception when the `WatchDogTimer` expires."""
//|
//|     RESET: WatchDogMode
//|     """Reset the system when the `WatchDogTimer` expires."""
//|
MAKE_ENUM_MAP(watchdog_watchdogmode) {
    MAKE_ENUM_MAP_ENTRY(watchdogmode, RAISE),
    MAKE_ENUM_MAP_ENTRY(watchdogmode, RESET),
};
static MP_DEFINE_CONST_DICT(watchdog_watchdogmode_locals_dict, watchdog_watchdogmode_locals_table);

MAKE_PRINTER(watchdog, watchdog_watchdogmode);

MAKE_ENUM_TYPE(watchdog, WatchDogMode, watchdog_watchdogmode);
