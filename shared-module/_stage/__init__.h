// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Radomir Dopieralski
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/busdisplay/BusDisplay.h"
#include <stdint.h>
#include <stdbool.h>
#include "py/obj.h"

#define TRANSPARENT (0x1ff8)

void render_stage(
    uint16_t x0, uint16_t y0,
    uint16_t x1, uint16_t y1,
    int16_t vx, int16_t vy,
    mp_obj_t *layers, size_t layers_size,
    uint16_t *buffer, size_t buffer_size,
    busdisplay_busdisplay_obj_t *display,
    uint8_t scale, uint16_t background);
