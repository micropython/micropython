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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_SHAPE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_SHAPE_H

#include "py/objproperty.h"
#include "py/objtuple.h"

#include "shared-bindings/vectorio/__init__.h"
#include "shared-module/vectorio/VectorShape.h"
#include "shared-module/displayio/area.h"

extern const mp_obj_type_t vectorio_vector_shape_type;

// Python shared bindings constructor
mp_obj_t vectorio_vector_shape_make_new(const mp_obj_t shape, const mp_obj_t pixel_shader, int32_t x, int32_t y);

// C data constructor
void common_hal_vectorio_vector_shape_construct(vectorio_vector_shape_t *self,
    vectorio_ishape_t ishape,
    mp_obj_t pixel_shader, int32_t x, int32_t y);

bool common_hal_vectorio_vector_shape_contains(vectorio_vector_shape_t *self, mp_int_t x, mp_int_t y);

void common_hal_vectorio_vector_shape_set_dirty(void *self);

mp_int_t common_hal_vectorio_vector_shape_get_x(vectorio_vector_shape_t *self);
void common_hal_vectorio_vector_shape_set_x(vectorio_vector_shape_t *self, mp_int_t x);

mp_obj_tuple_t *common_hal_vectorio_vector_shape_get_location(vectorio_vector_shape_t *self);
void common_hal_vectorio_vector_shape_set_location(vectorio_vector_shape_t *self, mp_obj_t xy);

mp_int_t common_hal_vectorio_vector_shape_get_y(vectorio_vector_shape_t *self);
void common_hal_vectorio_vector_shape_set_y(vectorio_vector_shape_t *self, mp_int_t y);

mp_int_t common_hal_vectorio_vector_shape_get_hidden(vectorio_vector_shape_t *self);
void common_hal_vectorio_vector_shape_set_hidden(vectorio_vector_shape_t *self, bool hidden);

mp_obj_t common_hal_vectorio_vector_shape_get_pixel_shader(vectorio_vector_shape_t *self);
void common_hal_vectorio_vector_shape_set_pixel_shader(vectorio_vector_shape_t *self, mp_obj_t pixel_shader);

void vectorio_vector_shape_update_transform(vectorio_vector_shape_t *self, displayio_buffer_transform_t *group_transform);

// Composable property definition for shapes that use VectorShape
extern vectorio_draw_protocol_impl_t vectorio_vector_shape_draw_protocol_impl;
extern const mp_obj_property_getset_t vectorio_vector_shape_x_obj;
extern const mp_obj_property_getset_t vectorio_vector_shape_y_obj;
extern const mp_obj_property_getset_t vectorio_vector_shape_hidden_obj;
extern const mp_obj_property_getset_t vectorio_vector_shape_location_obj;
extern const mp_obj_property_getset_t vectorio_vector_shape_pixel_shader_obj;
extern const mp_obj_fun_builtin_fixed_t vectorio_vector_shape_contains_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_VECTORIO_SHAPE_H
