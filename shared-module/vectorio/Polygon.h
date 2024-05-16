/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 by kvc0/WarriorOfWire
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_POLYGON_H
#define MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_POLYGON_H

#include <stdint.h>

#include "py/obj.h"
#include "shared-module/vectorio/__init__.h"

typedef struct {
    mp_obj_base_t base;
    // An int array[ x, y, ... ]
    int16_t *points_list;
    uint16_t len;
    uint16_t color_index;
    vectorio_event_t on_dirty;
    mp_obj_t draw_protocol_instance;
} vectorio_polygon_t;

#endif // MICROPY_INCLUDED_SHARED_MODULE_VECTORIO_POLYGON_H
