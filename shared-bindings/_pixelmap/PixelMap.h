// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Rose Hooper
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once
#include "py/obj.h"

extern const mp_obj_type_t pixelmap_pixelmap_type;

typedef struct _pixelmap_pixelmap_obj pixelmap_pixelmap_obj_t;

void shared_module_pixelmap_pixelmap_construct(pixelmap_pixelmap_obj_t *self, mp_obj_t pixelbuf, mp_obj_t indices);
bool shared_module_pixelmap_pixelmap_auto_write_get(pixelmap_pixelmap_obj_t *self);
void shared_module_pixelmap_pixelmap_auto_write_set(pixelmap_pixelmap_obj_t *self, bool auto_write);
void shared_module_pixelmap_pixelmap_fill(pixelmap_pixelmap_obj_t *self, const mp_obj_t color);
mp_obj_t shared_module_pixelmap_pixelmap_indices(pixelmap_pixelmap_obj_t *self, int index);
void shared_module_pixelmap_pixelmap_setslice(pixelmap_pixelmap_obj_t *self, const mp_obj_t value, mp_bound_slice_t slice, size_t slice_len);
mp_obj_t shared_module_pixelmap_pixelmap_getslice(pixelmap_pixelmap_obj_t *self, mp_bound_slice_t slice, size_t slice_len);
mp_obj_t shared_module_pixelmap_pixelmap_getitem(pixelmap_pixelmap_obj_t *self, mp_int_t index);
void shared_module_pixelmap_pixelmap_setitem(pixelmap_pixelmap_obj_t *self, mp_int_t index, const mp_obj_t value);
