/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Artyom Skrobov
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

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/proto.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-bindings/synthio/Math.h"
#include "shared-module/synthio/Math.h"

static const mp_arg_t math_properties[4];
STATIC mp_obj_t synthio_math_make_new_common(mp_arg_val_t args[MP_ARRAY_SIZE(math_properties)]);

MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, SUM, OP_SUM);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, ADD_SUB, OP_ADD_SUB);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, PRODUCT, OP_PRODUCT);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, MUL_DIV, OP_MUL_DIV);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, SCALE_OFFSET, OP_SCALE_OFFSET);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, OFFSET_SCALE, OP_OFFSET_SCALE);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, LERP, OP_LERP);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, CONSTRAINED_LERP, OP_CONSTRAINED_LERP);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, DIV_ADD, OP_DIV_ADD);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, ADD_DIV, OP_ADD_DIV);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, MID, OP_MID);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, MAX, OP_MAX);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, MIN, OP_MIN);
MAKE_ENUM_VALUE(synthio_math_operation_type, math_op, ABS, OP_ABS);

//| class MathOperation:
//|     """Operation for a Math block"""
//|
//|     def __call__(self, a: BlockInput, b: BlockInput = 0.0, c: BlockInput = 1.0) -> Math:
//|         """A MathOperation enumeration value can be called to construct a Math block that performs that operation"""
//|     SUM: "MathOperation"
//|     """Computes ``a+b+c``. For 2-input sum, set one argument to ``0.0``. To hold a control value for multiple subscribers, set two arguments to ``0.0``."""
//|
//|     ADD_SUB: "MathOperation"
//|     """Computes ``a+b-c``. For 2-input subtraction, set ``b`` to ``0.0``."""
//|
//|     PRODUCT: "MathOperation"
//|     """Computes ``a*b*c``. For 2-input product, set one argument to ``1.0``."""
//|
//|     MUL_DIV: "MathOperation"
//|     """Computes ``a*b/c``. If ``c`` is zero, the output is ``1.0``."""
//|
//|     SCALE_OFFSET: "MathOperation"
//|     """Computes ``(a*b)+c``."""
//|
//|     OFFSET_SCALE: "MathOperation"
//|     """Computes ``(a+b)*c``. For 2-input multiplication, set ``b`` to 0."""
//|
//|     LERP: "MathOperation"
//|     """Computes ``a * (1-c) + b * c``."""
//|
//|     CONSTRAINED_LERP: "MathOperation"
//|     """Computes ``a * (1-c') + b * c'``, where ``c'`` is constrained to be between ``0.0`` and ``1.0``."""
//|
//|     DIV_ADD: "MathOperation"
//|     """Computes ``a/b+c``.  If ``b`` is zero, the output is ``c``."""
//|
//|     ADD_DIV: "MathOperation"
//|     """Computes ``(a+b)/c``.  For 2-input product, set ``b`` to ``0.0``."""
//|
//|     MID: "MathOperation"
//|     """Returns the middle of the 3 input values."""
//|
//|     MAX: "MathOperation"
//|     """Returns the biggest of the 3 input values."""
//|
//|     MIN: "MathOperation"
//|     """Returns the smallest of the 3 input values."""
//|
//|     ABS: "MathOperation"
//|     """Returns the absolute value of ``a``."""
//|
MAKE_ENUM_MAP(synthio_math_operation) {
    MAKE_ENUM_MAP_ENTRY(math_op, SUM),
    MAKE_ENUM_MAP_ENTRY(math_op, ADD_SUB),
    MAKE_ENUM_MAP_ENTRY(math_op, PRODUCT),
    MAKE_ENUM_MAP_ENTRY(math_op, MUL_DIV),
    MAKE_ENUM_MAP_ENTRY(math_op, SCALE_OFFSET),
    MAKE_ENUM_MAP_ENTRY(math_op, OFFSET_SCALE),
    MAKE_ENUM_MAP_ENTRY(math_op, LERP),
    MAKE_ENUM_MAP_ENTRY(math_op, CONSTRAINED_LERP),
    MAKE_ENUM_MAP_ENTRY(math_op, DIV_ADD),
    MAKE_ENUM_MAP_ENTRY(math_op, ADD_DIV),
    MAKE_ENUM_MAP_ENTRY(math_op, MID),
    MAKE_ENUM_MAP_ENTRY(math_op, MAX),
    MAKE_ENUM_MAP_ENTRY(math_op, MIN),
    MAKE_ENUM_MAP_ENTRY(math_op, ABS),
};


STATIC mp_obj_t mathop_call(mp_obj_t fun, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_val_t args[4];
    args[0].u_obj = fun;
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(math_properties) - 1, math_properties + 1, &args[1]);
    return synthio_math_make_new_common(args);
}

STATIC MP_DEFINE_CONST_DICT(synthio_math_operation_locals_dict, synthio_math_operation_locals_table);
MAKE_PRINTER(synthio, synthio_math_operation);
MAKE_ENUM_TYPE(synthio, MathOperation, synthio_math_operation,
    call, mathop_call
    );

//| class Math:
//|     """An arithmetic block
//|
//|     Performs an arithmetic operation on up to 3 inputs. See the
//|     documentation of ``MathOperation`` for the specific functions available.
//|
//|     The properties can all be changed at run-time.
//|
//|     An Math only updates if it is actually associated with a playing `Synthesizer`,
//|     including indirectly via a `Note` or another intermediate Math.
//|
//|     Using the same Math as an input to multiple other Maths or Notes is OK, but
//|     the result if an Math is tied to multiple Synthtesizer objects is undefined.
//|
//|     In the current implementation, Maths are updated every 256 samples. This
//|     should be considered an implementation detail.
//|     """
//|
//|     def __init__(
//|         self,
//|         operation: MathOperation,
//|         a: BlockInput,
//|         b: BlockInput = 0.0,
//|         c: BlockInput = 1.0,
//|     ):
//|         pass
static const mp_arg_t math_properties[] = {
    { MP_QSTR_operation, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = NULL } },
    { MP_QSTR_a, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = NULL } },
    { MP_QSTR_b, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(0) } },
    { MP_QSTR_c, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(1) } },
};

STATIC mp_obj_t synthio_math_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(math_properties)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(math_properties), math_properties, args);

    return synthio_math_make_new_common(args);
}

STATIC mp_obj_t synthio_math_make_new_common(mp_arg_val_t args[MP_ARRAY_SIZE(math_properties)]) {
    synthio_math_obj_t *self = mp_obj_malloc(synthio_math_obj_t, &synthio_math_type);

    self->base.last_tick = synthio_global_tick;

    mp_obj_t result = MP_OBJ_FROM_PTR(self);
    properties_construct_helper(result, math_properties, args, MP_ARRAY_SIZE(math_properties));

    return result;
};

//|     a: BlockInput
//|     """The first input to the operation"""
STATIC mp_obj_t synthio_math_get_a(mp_obj_t self_in) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_math_get_input_obj(self, 0);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_math_get_a_obj, synthio_math_get_a);

STATIC mp_obj_t synthio_math_set_a(mp_obj_t self_in, mp_obj_t arg) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_math_set_input_obj(self, 0, arg, MP_QSTR_a);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_math_set_a_obj, synthio_math_set_a);
MP_PROPERTY_GETSET(synthio_math_a_obj,
    (mp_obj_t)&synthio_math_get_a_obj,
    (mp_obj_t)&synthio_math_set_a_obj);


//|     b: BlockInput
//|     """The second input to the operation"""
STATIC mp_obj_t synthio_math_get_b(mp_obj_t self_in) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_math_get_input_obj(self, 1);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_math_get_b_obj, synthio_math_get_b);

STATIC mp_obj_t synthio_math_set_b(mp_obj_t self_in, mp_obj_t arg) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_math_set_input_obj(self, 1, arg, MP_QSTR_b);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_math_set_b_obj, synthio_math_set_b);
MP_PROPERTY_GETSET(synthio_math_b_obj,
    (mp_obj_t)&synthio_math_get_b_obj,
    (mp_obj_t)&synthio_math_set_b_obj);


//|     c: BlockInput
//|     """The third input to the operation"""
STATIC mp_obj_t synthio_math_get_c(mp_obj_t self_in) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_math_get_input_obj(self, 2);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_math_get_c_obj, synthio_math_get_c);

STATIC mp_obj_t synthio_math_set_c(mp_obj_t self_in, mp_obj_t arg) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_math_set_input_obj(self, 2, arg, MP_QSTR_c);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_math_set_c_obj, synthio_math_set_c);
MP_PROPERTY_GETSET(synthio_math_c_obj,
    (mp_obj_t)&synthio_math_get_c_obj,
    (mp_obj_t)&synthio_math_set_c_obj);


//|     operation: MathOperation
//|     """The function to compute"""
STATIC mp_obj_t synthio_math_get_operation(mp_obj_t self_in) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return cp_enum_find(&synthio_math_operation_type, common_hal_synthio_math_get_operation(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_math_get_operation_obj, synthio_math_get_operation);

STATIC mp_obj_t synthio_math_set_operation(mp_obj_t self_in, mp_obj_t arg) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_math_set_operation(self, cp_enum_value(&synthio_math_operation_type, arg, MP_QSTR_operation));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_math_set_operation_obj, synthio_math_set_operation);
MP_PROPERTY_GETSET(synthio_math_operation_obj,
    (mp_obj_t)&synthio_math_get_operation_obj,
    (mp_obj_t)&synthio_math_set_operation_obj);



//|
//|     value: float
//|     """The value of the oscillator (read-only)"""
//|
STATIC mp_obj_t synthio_math_get_value(mp_obj_t self_in) {
    synthio_math_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_math_get_value(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_math_get_value_obj, synthio_math_get_value);

MP_PROPERTY_GETTER(synthio_math_value_obj,
    (mp_obj_t)&synthio_math_get_value_obj);


static void math_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    properties_print_helper(print, self_in, math_properties, MP_ARRAY_SIZE(math_properties));
}

STATIC const mp_rom_map_elem_t synthio_math_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_a), MP_ROM_PTR(&synthio_math_a_obj) },
    { MP_ROM_QSTR(MP_QSTR_b), MP_ROM_PTR(&synthio_math_b_obj) },
    { MP_ROM_QSTR(MP_QSTR_c), MP_ROM_PTR(&synthio_math_c_obj) },
    { MP_ROM_QSTR(MP_QSTR_operation), MP_ROM_PTR(&synthio_math_operation_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&synthio_math_value_obj) },
};
STATIC MP_DEFINE_CONST_DICT(synthio_math_locals_dict, synthio_math_locals_dict_table);


STATIC const synthio_block_proto_t math_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_synthio_block)
    .tick = common_hal_synthio_math_tick,
};

MP_DEFINE_CONST_OBJ_TYPE(
    synthio_math_type,
    MP_QSTR_Math,
    MP_TYPE_FLAG_NONE,
    make_new, synthio_math_make_new,
    locals_dict, &synthio_math_locals_dict,
    print, math_print,
    protocol, &math_proto
    );
