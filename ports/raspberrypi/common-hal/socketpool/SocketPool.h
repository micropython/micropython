// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#if CIRCUITPY_AIRLIFT
#include "shared-module/_airlift/socketpool.h"
#else

#include "lwip/ip_addr.h"

typedef struct {
    mp_obj_base_t base;
} socketpool_socketpool_obj_t;

#endif
