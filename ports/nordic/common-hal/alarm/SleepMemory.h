// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Junji Sakai
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#define SLEEP_MEMORY_LENGTH (256)

typedef struct {
    mp_obj_base_t base;
} alarm_sleep_memory_obj_t;

extern void set_memory_retention(void);
extern void alarm_sleep_memory_reset(void);
