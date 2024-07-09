// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    bool console;
    bool display;
    bool updated;
} supervisor_status_bar_obj_t;

extern void shared_module_supervisor_status_bar_init(supervisor_status_bar_obj_t *self);
extern void shared_module_supervisor_status_bar_updated(supervisor_status_bar_obj_t *self);
