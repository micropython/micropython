// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"

#include "shared-module/vectorio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    uint16_t radius;
    uint16_t color_index;
    vectorio_event_t on_dirty;
    mp_obj_t draw_protocol_instance;
} vectorio_circle_t;
