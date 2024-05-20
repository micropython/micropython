// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Artyom Skrobov
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/proto.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-bindings/synthio/LFO.h"
#include "shared-module/synthio/LFO.h"

static const uint16_t triangle[] = {0, 32767, 0, -32767};

//| class LFO:
//|     """A low-frequency oscillator block
//|
//|     Every `rate` seconds, the output of the LFO cycles through its `waveform`.
//|     The output at any particular moment is ``waveform[idx] * scale + offset``.
//|
//|     If `waveform` is None, a triangle waveform is used.
//|
//|     `rate`, `phase_offset`, `offset`, `scale`, and `once` can be changed at
//|     run-time. `waveform` may be mutated.
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
//|
//|     An LFO's ``value`` property is computed once when it is constructed, and then
//|     when its associated synthesizer updates it.
//|
//|     This means that for instance an LFO **created** with ``offset=1`` has ```value==1``
//|     immediately, but **updating** the ``offset`` property alone does not
//|     change ``value``; it only updates through an association with an active synthesizer.
//|
//|     The interpolation of the waveform is necessarily different depending on the
//|     ``once`` property. Consider a LFO with ``waveform=np.array([0, 100],
//|     dtype=np.int16), interpolate=True, once=True, rate=1``. Over 1 second this
//|     LFO's output will change from ``0`` to ``100``, and will remain at
//|     ``100`` thereafter, creating a "bend out" over a duration of 1 second.
//|
//|     However, when ``once=False``, this creates a triangle waveform with a
//|     period of 1 second. Over about the first half second the input will
//|     increase from ``0`` to ``100``, then during the second half of the second
//|     it will decrease back to ``0``.
//|
//|     The time of the peak output is different depending on the value of ``once``:
//|     At 1.0s for ``once=True`` and at 0.5s for ``once=False``.
//|
//|     Because of this difference in interpolation, dynamically updating the
//|     ``once`` flag except when the LFO is at a phase of 0 will cause a step in
//|     the LFO's output.
//|     """
//|
//|     def __init__(
//|         self,
//|         waveform: Optional[ReadableBuffer] = None,
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

static mp_obj_t synthio_lfo_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_waveform }; // others never directly referred to by argument number

    mp_arg_val_t args[MP_ARRAY_SIZE(lfo_properties)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(lfo_properties), lfo_properties, args);

    synthio_lfo_obj_t *self = mp_obj_malloc(synthio_lfo_obj_t, &synthio_lfo_type);

    self->waveform_bufinfo = ((mp_buffer_info_t) {.buf = (void *)triangle, .len = MP_ARRAY_SIZE(triangle)});
    if (args[ARG_waveform].u_obj != mp_const_none) {
        synthio_synth_parse_waveform(&self->waveform_bufinfo, args[ARG_waveform].u_obj);
    }
    self->waveform_obj = args[ARG_waveform].u_obj;

    mp_obj_t result = MP_OBJ_FROM_PTR(self);
    properties_construct_helper(result, lfo_properties + 1, args + 1, MP_ARRAY_SIZE(lfo_properties) - 1);

    // Force computation of the LFO's initial output
    synthio_global_rate_scale = 0;
    self->base.last_tick = synthio_global_tick - 1;
    synthio_block_slot_t slot;
    synthio_block_assign_slot(MP_OBJ_FROM_PTR(result), &slot, MP_QSTR_self);
    (void)synthio_block_slot_get(&slot);

    return result;
};

//|     waveform: Optional[ReadableBuffer]
//|     """The waveform of this lfo. (read-only, but the values in the buffer may be modified dynamically)"""
static mp_obj_t synthio_lfo_get_waveform(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_waveform_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_waveform_obj, synthio_lfo_get_waveform);

MP_PROPERTY_GETTER(synthio_lfo_waveform_obj,
    (mp_obj_t)&synthio_lfo_get_waveform_obj);

//|     rate: BlockInput
//|     """The rate (in Hz) at which the LFO cycles through its waveform"""
static mp_obj_t synthio_lfo_get_rate(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_rate_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_rate_obj, synthio_lfo_get_rate);

static mp_obj_t synthio_lfo_set_rate(mp_obj_t self_in, mp_obj_t arg) {
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
static mp_obj_t synthio_lfo_get_offset(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_offset_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_offset_obj, synthio_lfo_get_offset);

static mp_obj_t synthio_lfo_set_offset(mp_obj_t self_in, mp_obj_t arg) {
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
static mp_obj_t synthio_lfo_get_phase_offset(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_phase_offset_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_phase_offset_obj, synthio_lfo_get_phase_offset);

static mp_obj_t synthio_lfo_set_phase_offset(mp_obj_t self_in, mp_obj_t arg) {
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
static mp_obj_t synthio_lfo_get_scale(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_lfo_get_scale_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_scale_obj, synthio_lfo_get_scale);

static mp_obj_t synthio_lfo_set_scale(mp_obj_t self_in, mp_obj_t arg) {
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
static mp_obj_t synthio_lfo_get_once(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_synthio_lfo_get_once(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_once_obj, synthio_lfo_get_once);

static mp_obj_t synthio_lfo_set_once(mp_obj_t self_in, mp_obj_t arg) {
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
static mp_obj_t synthio_lfo_get_interpolate(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_synthio_lfo_get_interpolate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_interpolate_obj, synthio_lfo_get_interpolate);

static mp_obj_t synthio_lfo_set_interpolate(mp_obj_t self_in, mp_obj_t arg) {
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
static mp_obj_t synthio_lfo_get_phase(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_lfo_get_phase(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_get_phase_obj, synthio_lfo_get_phase);

MP_PROPERTY_GETTER(synthio_lfo_phase_obj,
    (mp_obj_t)&synthio_lfo_get_phase_obj);


//|
//|     value: float
//|     """The value of the oscillator (read-only)"""
static mp_obj_t synthio_lfo_get_value(mp_obj_t self_in) {
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
static mp_obj_t synthio_lfo_retrigger(mp_obj_t self_in) {
    synthio_lfo_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_lfo_retrigger(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_lfo_retrigger_obj, synthio_lfo_retrigger);

static void lfo_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    properties_print_helper(print, self_in, lfo_properties, MP_ARRAY_SIZE(lfo_properties));
}

static const mp_rom_map_elem_t synthio_lfo_locals_dict_table[] = {
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
static MP_DEFINE_CONST_DICT(synthio_lfo_locals_dict, synthio_lfo_locals_dict_table);


static const synthio_block_proto_t lfo_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_synthio_block)
    .tick = common_hal_synthio_lfo_tick,
};

MP_DEFINE_CONST_OBJ_TYPE(
    synthio_lfo_type,
    MP_QSTR_LFO,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, synthio_lfo_make_new,
    locals_dict, &synthio_lfo_locals_dict,
    print, lfo_print,
    protocol, &lfo_proto
    );
