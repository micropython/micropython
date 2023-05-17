/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include <math.h>

#include "shared-bindings/synthio/Math.h"
#include "shared-module/synthio/Math.h"

mp_obj_t common_hal_synthio_math_get_input_obj(synthio_math_obj_t *self, size_t i) {
    return self->inputs[i].obj;
}

void common_hal_synthio_math_set_input_obj(synthio_math_obj_t *self, size_t i, mp_obj_t arg, qstr argname) {
    assert(i < MP_ARRAY_SIZE(self->inputs));
    synthio_block_assign_slot(arg, &self->inputs[i], argname);
}

synthio_math_operation_t common_hal_synthio_math_get_operation(synthio_math_obj_t *self) {
    return self->operation;
}

void common_hal_synthio_math_set_operation(synthio_math_obj_t *self, synthio_math_operation_t arg) {
    self->operation = arg;
}

#define ZERO (MICROPY_FLOAT_CONST(0.))

mp_float_t common_hal_synthio_math_get_value(synthio_math_obj_t *self) {
    return self->base.value;
}

mp_float_t common_hal_synthio_math_tick(mp_obj_t self_in) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_float_t a = synthio_block_slot_get(&self->inputs[0]);

    if (self->operation == OP_ABS) {
        return MICROPY_FLOAT_C_FUN(fabs)(a);
    }

    mp_float_t b = synthio_block_slot_get(&self->inputs[1]);
    mp_float_t c = synthio_block_slot_get(&self->inputs[2]);

    switch (self->operation) {
        case OP_SUM:
            return a + b + c;
        case OP_ADD_SUB:
            return a + b - c;
        case OP_PRODUCT:
            return a * b * c;
        case OP_MUL_DIV:
            if (fpclassify(c) == FP_ZERO) {
                return 0;
            }
            return a * b / c;
        case OP_SCALE_OFFSET:
            return a * b + c;
        case OP_OFFSET_SCALE:
            return (a + b) * c;
        case OP_CONSTRAINED_LERP:
            c = MIN(1, MAX(0, c));
            MP_FALLTHROUGH;
        case OP_LERP:
            return a * (1 - c) + b * c;
        case OP_DIV_ADD:
            if (fpclassify(b) == FP_ZERO) {
                return ZERO;
            }
            return a / b + c;
        case OP_ADD_DIV:
            if (fpclassify(c) == FP_ZERO) {
                return ZERO;
            }
            return (a + b) / c;
        case OP_MID:
            if (a < b) {
                if (b < c) {
                    return b;
                }
                if (a < c) {
                    return c;
                }
                return a;
            }
            if (a < c) {
                return a;
            }
            if (c < b) {
                return b;
            }
            return c;
        case OP_MIN:
            return MIN(a,MIN(b,c));
        case OP_MAX:
            return MAX(a,MAX(b,c));
        case OP_ABS:
            break;
    }
    return ZERO;
}
