// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
