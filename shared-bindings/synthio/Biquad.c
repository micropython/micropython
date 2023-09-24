/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Artyom Skrobov
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
#include <string.h>

#include "py/enum.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/objnamedtuple.h"
#include "py/runtime.h"

#include "shared-bindings/synthio/__init__.h"
#include "shared-bindings/synthio/LFO.h"
#include "shared-bindings/synthio/Math.h"
#include "shared-bindings/synthio/MidiTrack.h"
#include "shared-bindings/synthio/Note.h"
#include "shared-bindings/synthio/Synthesizer.h"

#include "shared-module/synthio/LFO.h"

#define default_attack_time (MICROPY_FLOAT_CONST(0.1))
#define default_decay_time (MICROPY_FLOAT_CONST(0.05))
#define default_release_time (MICROPY_FLOAT_CONST(0.2))
#define default_attack_level (MICROPY_FLOAT_CONST(1.))
#define default_sustain_level (MICROPY_FLOAT_CONST(0.8))

static const mp_arg_t biquad_properties[] = {
    { MP_QSTR_a1, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_ROM_NONE} },
    { MP_QSTR_a2, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_ROM_NONE} },
    { MP_QSTR_b0, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_ROM_NONE} },
    { MP_QSTR_b1, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_ROM_NONE} },
    { MP_QSTR_b2, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_ROM_NONE} },
};

//| class Biquad:
//|     def __init__(self, b0: float, b1: float, b2: float, a1: float, a2: float) -> None:
//|         """Construct a normalized biquad filter object.
//|
//|         This implements the "direct form 1" biquad filter, where each coefficient
//|         has been pre-divided by a0.
//|
//|         Biquad objects are usually constructed via one of the related methods on a `Synthesizer` object
//|         rather than directly from coefficients.
//|
//|         https://github.com/WebAudio/Audio-EQ-Cookbook/blob/main/Audio-EQ-Cookbook.txt
//|         """
//|
STATIC mp_obj_t synthio_biquad_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(biquad_properties)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(biquad_properties), biquad_properties, args);

    for (size_t i = 0; i < MP_ARRAY_SIZE(biquad_properties); i++) {
        args[i].u_obj = mp_obj_new_float(mp_arg_validate_type_float(args[i].u_obj, biquad_properties[i].qst));
    }

    MP_STATIC_ASSERT(sizeof(mp_arg_val_t) == sizeof(mp_obj_t));
    return namedtuple_make_new(type_in, MP_ARRAY_SIZE(args), 0, &args[0].u_obj);
}

const mp_obj_namedtuple_type_t synthio_biquad_type_obj = {
    .base = {
        .base = {
            .type = &mp_type_type
        },
        .flags = MP_TYPE_FLAG_EXTENDED,
        .name = MP_QSTR_Biquad,
        .print = namedtuple_print,
        .parent = &mp_type_tuple,
        .make_new = synthio_biquad_make_new,
        .attr = namedtuple_attr,
        MP_TYPE_EXTENDED_FIELDS(
            .unary_op = mp_obj_tuple_unary_op,
            .binary_op = mp_obj_tuple_binary_op,
            .subscr = mp_obj_tuple_subscr,
            .getiter = mp_obj_tuple_getiter,
            ),
    },
    .n_fields = 5,
    .fields = {
        MP_QSTR_a1,
        MP_QSTR_a2,
        MP_QSTR_b0,
        MP_QSTR_b1,
        MP_QSTR_b2,
    },
};
