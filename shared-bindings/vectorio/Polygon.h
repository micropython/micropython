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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_POLYGON_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_POLYGON_H

#include "shared-module/vectorio/Polygon.h"
#include "shared-module/displayio/area.h"
#include "shared-module/vectorio/__init__.h"

extern const mp_obj_type_t vectorio_polygon_type;

void common_hal_vectorio_polygon_construct(vectorio_polygon_t *self, mp_obj_t points_list, uint16_t color_index);
void common_hal_vectorio_polygon_set_on_dirty(vectorio_polygon_t *self, vectorio_event_t notification);


uint32_t common_hal_vectorio_polygon_get_pixel(void *polygon, int16_t x, int16_t y);

void common_hal_vectorio_polygon_get_area(void *polygon, displayio_area_t *out_area);



mp_obj_t common_hal_vectorio_polygon_get_points(vectorio_polygon_t *self);
void common_hal_vectorio_polygon_set_points(vectorio_polygon_t *self, mp_obj_t points_list);

uint16_t common_hal_vectorio_polygon_get_color_index(void *obj);
void common_hal_vectorio_polygon_set_color_index(void *obj, uint16_t color_index);

mp_obj_t common_hal_vectorio_polygon_get_draw_protocol(void *polygon);


#endif // MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_POLYGON_H
