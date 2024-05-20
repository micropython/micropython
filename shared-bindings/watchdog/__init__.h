// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objexcept.h"

extern const mp_obj_module_t watchdog_module;
extern mp_obj_exception_t mp_watchdog_timeout_exception;
extern const mp_obj_type_t mp_type_WatchDogTimeout;
