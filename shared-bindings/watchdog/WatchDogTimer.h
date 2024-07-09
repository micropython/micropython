// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-bindings/watchdog/WatchDogMode.h"

typedef struct _watchdog_watchdogtimer_obj_t watchdog_watchdogtimer_obj_t;

extern void common_hal_watchdog_feed(watchdog_watchdogtimer_obj_t *self);

extern void common_hal_watchdog_set_mode(watchdog_watchdogtimer_obj_t *self, watchdog_watchdogmode_t mode);
extern watchdog_watchdogmode_t common_hal_watchdog_get_mode(watchdog_watchdogtimer_obj_t *self);

extern void common_hal_watchdog_set_timeout(watchdog_watchdogtimer_obj_t *self, mp_float_t timeout);
extern mp_float_t common_hal_watchdog_get_timeout(watchdog_watchdogtimer_obj_t *self);

extern void common_hal_watchdog_enable(watchdog_watchdogtimer_obj_t *self);
extern void common_hal_watchdog_deinit(watchdog_watchdogtimer_obj_t *self);

extern const mp_obj_type_t watchdog_watchdogtimer_type;
