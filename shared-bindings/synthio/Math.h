/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Jeff Epler for Adafruit Industries
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

#pragma once

#include "py/obj.h"

typedef enum {
    OP_SUM,
    OP_ADD_SUB,
    OP_PRODUCT,
    OP_MUL_DIV,
    OP_SCALE_OFFSET,
    OP_OFFSET_SCALE,
    OP_LERP,
    OP_CONSTRAINED_LERP,
    OP_DIV_ADD,
    OP_ADD_DIV,
    OP_MID,
    OP_MIN,
    OP_MAX,
    OP_ABS
} synthio_math_operation_t;

typedef struct synthio_math_obj synthio_math_obj_t;
extern const mp_obj_type_t synthio_math_type;
extern const mp_obj_type_t synthio_math_operation_type;

mp_obj_t common_hal_synthio_math_get_input_obj(synthio_math_obj_t *self, size_t i);
void common_hal_synthio_math_set_input_obj(synthio_math_obj_t *self, size_t i, mp_obj_t arg, qstr argname);

synthio_math_operation_t common_hal_synthio_math_get_operation(synthio_math_obj_t *self);
void common_hal_synthio_math_set_operation(synthio_math_obj_t *self, synthio_math_operation_t arg);

mp_float_t common_hal_synthio_math_get_value(synthio_math_obj_t *self);

mp_float_t common_hal_synthio_math_tick(mp_obj_t self_in);
