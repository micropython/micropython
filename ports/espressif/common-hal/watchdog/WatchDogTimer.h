// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_WATCHDOG_WATCHDOGTIMER_H
#define MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_WATCHDOG_WATCHDOGTIMER_H

#include "py/obj.h"

#include "shared-module/watchdog/__init__.h"

#include "shared-bindings/watchdog/WatchDogMode.h"
#include "shared-bindings/watchdog/WatchDogTimer.h"

struct _watchdog_watchdogtimer_obj_t {
    mp_obj_base_t base;
    mp_float_t timeout;
    watchdog_watchdogmode_t mode;
};

#endif // MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_WATCHDOG_WATCHDOGTIMER_H
