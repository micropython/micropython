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
#include "shared-bindings/synthio/LFO.h"
#include "shared-module/synthio/LFO.h"

STATIC const uint16_t triangle[] = {0, 32767, 0, -32767};

//| class LFO:
//|     """A low-frequency oscillator block
//|
//|     Every `rate` seconds, the output of the LFO cycles through its `waveform`.
//|     The output at any particular moment is ``waveform[idx] * scale + offset``.
//|
//|     If `waveform` is None, a triangle waveform is used.
//|
//|     `rate`, `phase_offset`, `offset`, `scale`, and `once` can be changed at run-time. `waveform` may be mutated.
//|
//|     `waveform` must be a ``ReadableBuffer`` with elements of type ``'h'``
//|     (16-bit signed integer).  Internally, the elements of `waveform` are scaled
//|     so that the input range ``[-32768,32767]`` maps to ``[-1.0, 0.99996]``.
//|
//|     An LFO only updates if it is actually associated with a playing `Synthesizer`,
//|     including indirectly via a `Note` or another intermediate LFO.
//|
//|     Using the same LFO as an input to multiple other LFOs or Notes is OK, but
//|     the result if an LFO is tied to multiple Synthtesizer objects is undefined.
//|
//|     In the current implementation, LFOs are updated every 256 samples. This
//|     should be considered an implementation detail, though it affects how LFOs
//|     behave for instance when used to implement an integrator (``l.offset = l``).
//|     """
//|
//|     def __init__(
//|         self,
//|         waveform: ReadableBuffer = None,
//|         *,
//|         rate: BlockInput = 1.0,
//|         scale: BlockInput = 1.0,
//|         offset: BlockInput = 0.0,
//|         phase_offset: BlockInput = 0.0,
//|         once=False,
//|         interpolate=True
//|     ):
//|         pass
static const mp_arg_t lfo_properties[] = {
    { MP_QSTR_waveform, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE } },
    { MP_QSTR_rate, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(1) } },
    { MP_QSTR_scale, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(1) } },
    { MP_QSTR_offset, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(0) } },
    { MP_QSTR_phase_offset, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(0) } },
    { MP_QSTR_once, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(0) } },
    { MP_QSTR_interpolate, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(1) } },
};

STATIC mp_obj_t synthio_lfo_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_waveform }; // others never directly referred to by argument number

    mp_arg_val_t args[MP_ARRAY_SIZE(lfo_properties)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(lfo_properties), lfo_properties, args);

    synthio_lfo_obj_t *self = mp_obj_malloc(synthio_lfo_obj_t, &synthio_lfo_type);

    self->waveform_bufinfo = ((mp_buffer_info_t) {.buf = (void *)triangle, .len = MP_ARRAY_SIZE(triangle)});
    if (args[ARG_waveform].u_obj != mp_const_none) {
        synthio_synth_parse_waveform(&self->waveform_bufinfo, args[ARG_waveform].u_obj);
    }
    self->waveform_obj = args[ARG_waveform].u_obj;
    self->base.last_tick = synthio_global_tick;

    mp_obj_t result = MP_OBJ_FROM_PTR(self);
    properties_construct_helper(result, lfo_properties + 1, args + 1, MP_ARRAY_SIZE(lfo_properties) - 1);

    return result;
};

//|     waveform: Optional[ReadableBuffer]
//|     """The waveform of this lfo. (read-only, but the values in the buffer may be modified dynamically)"""
STATIC mp_obj_t synthio_lfo_get_waveform(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_waveform_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_waveform_obj, synthio_lfo_get_waveform);

MP_PROPERTY_GETTER(synthio_lfo_waveform_obj,
    (mp_obj_t)&synthio_lfo_get_waveform_obj);

//|     rate: BlockInput
//|     """The rate (in Hz) at which the LFO cycles through its waveform"""
STATIC mp_obj_t synthio_lfo_get_rate(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_rate_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_rate_obj, synthio_lfo_get_rate);

STATIC mp_obj_t synthio_lfo_set_rate(mp_obj_t self_in, mp_obj_t arg) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_lfo_set_rate_obj(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_lfo_set_rate_obj, synthio_lfo_set_rate);
MP_PROPERTY_GETSET(synthio_lfo_rate_obj,
    (mp_obj_t)&synthio_lfo_get_rate_obj,
    (mp_obj_t)&synthio_lfo_set_rate_obj);


//|     offset: BlockInput
//|     """An additive value applied to the LFO's output"""
STATIC mp_obj_t synthio_lfo_get_offset(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_offset_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_offset_obj, synthio_lfo_get_offset);

STATIC mp_obj_t synthio_lfo_set_offset(mp_obj_t self_in, mp_obj_t arg) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_lfo_set_offset_obj(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_lfo_set_offset_obj, synthio_lfo_set_offset);
MP_PROPERTY_GETSET(synthio_lfo_offset_obj,
    (mp_obj_t)&synthio_lfo_get_offset_obj,
    (mp_obj_t)&synthio_lfo_set_offset_obj);

//|     phase_offset: BlockInput
//|     """An additive value applied to the LFO's phase"""
STATIC mp_obj_t synthio_lfo_get_phase_offset(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_phase_offset_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_phase_offset_obj, synthio_lfo_get_phase_offset);

STATIC mp_obj_t synthio_lfo_set_phase_offset(mp_obj_t self_in, mp_obj_t arg) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_lfo_set_phase_offset_obj(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_lfo_set_phase_offset_obj, synthio_lfo_set_phase_offset);
MP_PROPERTY_GETSET(synthio_lfo_phase_offset_obj,
    (mp_obj_t)&synthio_lfo_get_phase_offset_obj,
    (mp_obj_t)&synthio_lfo_set_phase_offset_obj);

//|     scale: BlockInput
//|     """An multiplier value applied to the LFO's output"""
STATIC mp_obj_t synthio_lfo_get_scale(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_scale_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_scale_obj, synthio_lfo_get_scale);

STATIC mp_obj_t synthio_lfo_set_scale(mp_obj_t self_in, mp_obj_t arg) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_lfo_set_scale_obj(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_lfo_set_scale_obj, synthio_lfo_set_scale);
MP_PROPERTY_GETSET(synthio_lfo_scale_obj,
    (mp_obj_t)&synthio_lfo_get_scale_obj,
    (mp_obj_t)&synthio_lfo_set_scale_obj);

//|
//|     once: bool
//|     """True if the waveform should stop when it reaches its last output value, false if it should re-start at the beginning of its waveform
//|
//|     This applies to the ``phase`` *before* the addition of any ``phase_offset`` """
STATIC mp_obj_t synthio_lfo_get_once(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_synthio_lfo_get_once(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_once_obj, synthio_lfo_get_once);

STATIC mp_obj_t synthio_lfo_set_once(mp_obj_t self_in, mp_obj_t arg) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_lfo_set_once(self, mp_obj_is_true(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_lfo_set_once_obj, synthio_lfo_set_once);
MP_PROPERTY_GETSET(synthio_lfo_once_obj,
    (mp_obj_t)&synthio_lfo_get_once_obj,
    (mp_obj_t)&synthio_lfo_set_once_obj);


//|
//|     interpolate: bool
//|     """True if the waveform should perform linear interpolation between values"""
STATIC mp_obj_t synthio_lfo_get_interpolate(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_synthio_lfo_get_interpolate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_interpolate_obj, synthio_lfo_get_interpolate);

STATIC mp_obj_t synthio_lfo_set_interpolate(mp_obj_t self_in, mp_obj_t arg) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_lfo_set_interpolate(self, mp_obj_is_true(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_lfo_set_interpolate_obj, synthio_lfo_set_interpolate);
MP_PROPERTY_GETSET(synthio_lfo_interpolate_obj,
    (mp_obj_t)&synthio_lfo_get_interpolate_obj,
    (mp_obj_t)&synthio_lfo_set_interpolate_obj);


//|
//|     phase: float
//|     """The phase of the oscillator, in the range 0 to 1 (read-only)"""
STATIC mp_obj_t synthio_lfo_get_phase(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_lfo_get_phase(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_phase_obj, synthio_lfo_get_phase);

MP_PROPERTY_GETTER(synthio_lfo_phase_obj,
    (mp_obj_t)&synthio_lfo_get_phase_obj);


//|
//|     value: float
//|     """The value of the oscillator (read-only)"""
STATIC mp_obj_t synthio_lfo_get_value(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_lfo_get_value(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_value_obj, synthio_lfo_get_value);

MP_PROPERTY_GETTER(synthio_lfo_value_obj,
    (mp_obj_t)&synthio_lfo_get_value_obj);


//|
//|     def retrigger(self):
//|         """Reset the LFO's internal index to the start of the waveform. Most useful when it its `once` property is `True`."""
//|
STATIC mp_obj_t synthio_lfo_retrigger(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_lfo_retrigger(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_retrigger_obj, synthio_lfo_retrigger);

static void lfo_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    properties_print_helper(print, self_in, lfo_properties, MP_ARRAY_SIZE(lfo_properties));
}

STATIC const mp_rom_map_elem_t synthio_lfo_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_waveform), MP_ROM_PTR(&synthio_lfo_waveform_obj) },
    { MP_ROM_QSTR(MP_QSTR_rate), MP_ROM_PTR(&synthio_lfo_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_scale), MP_ROM_PTR(&synthio_lfo_scale_obj) },
    { MP_ROM_QSTR(MP_QSTR_offset), MP_ROM_PTR(&synthio_lfo_offset_obj) },
    { MP_ROM_QSTR(MP_QSTR_phase_offset), MP_ROM_PTR(&synthio_lfo_phase_offset_obj) },
    { MP_ROM_QSTR(MP_QSTR_once), MP_ROM_PTR(&synthio_lfo_once_obj) },
    { MP_ROM_QSTR(MP_QSTR_interpolate), MP_ROM_PTR(&synthio_lfo_interpolate_obj) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&synthio_lfo_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_phase), MP_ROM_PTR(&synthio_lfo_phase_obj) },
    { MP_ROM_QSTR(MP_QSTR_retrigger), MP_ROM_PTR(&synthio_lfo_retrigger_obj) },
};
STATIC MP_DEFINE_CONST_DICT(synthio_lfo_locals_dict, synthio_lfo_locals_dict_table);


STATIC const synthio_block_proto_t lfo_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_synthio_block)
    .tick = common_hal_synthio_lfo_tick,
};

const mp_obj_type_t synthio_lfo_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_LFO,
    .make_new = synthio_lfo_make_new,
    .locals_dict = (mp_obj_dict_t *)&synthio_lfo_locals_dict,
    .print = lfo_print,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &lfo_proto,
        ),
};
