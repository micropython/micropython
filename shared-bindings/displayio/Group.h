/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_GROUP_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_GROUP_H

#include "shared-module/displayio/Group.h"

extern const mp_obj_type_t displayio_group_type;

displayio_group_t *native_group(mp_obj_t group_obj);

void common_hal_displayio_group_construct(displayio_group_t *self, uint32_t scale, mp_int_t x, mp_int_t y);
uint32_t common_hal_displayio_group_get_scale(displayio_group_t *self);
void common_hal_displayio_group_set_scale(displayio_group_t *self, uint32_t scale);
bool common_hal_displayio_group_get_hidden(displayio_group_t *self);
void common_hal_displayio_group_set_hidden(displayio_group_t *self, bool hidden);
mp_int_t common_hal_displayio_group_get_x(displayio_group_t *self);
void common_hal_displayio_group_set_x(displayio_group_t *self, mp_int_t x);
mp_int_t common_hal_displayio_group_get_y(displayio_group_t *self);
void common_hal_displayio_group_set_y(displayio_group_t *self, mp_int_t y);
void common_hal_displayio_group_append(displayio_group_t *self, mp_obj_t layer);
void common_hal_displayio_group_insert(displayio_group_t *self, size_t index, mp_obj_t layer);
size_t common_hal_displayio_group_get_len(displayio_group_t *self);
mp_obj_t common_hal_displayio_group_pop(displayio_group_t *self, size_t index);
mp_int_t common_hal_displayio_group_index(displayio_group_t *self, mp_obj_t layer);
mp_obj_t common_hal_displayio_group_get(displayio_group_t *self, size_t index);
void common_hal_displayio_group_set(displayio_group_t *self, size_t index, mp_obj_t layer);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYIO_GROUP_H
