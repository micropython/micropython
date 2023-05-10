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

#include <stdint.h>

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-bindings/synthio/__init__.h"
#include "shared-bindings/synthio/Note.h"
#include "shared-module/synthio/Note.h"

static const mp_arg_t note_properties[] = {
    { MP_QSTR_frequency, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = NULL } },
    { MP_QSTR_panning, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(0) } },
    { MP_QSTR_tremolo_rate, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = NULL } },
    { MP_QSTR_tremolo_depth, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = NULL } },
    { MP_QSTR_bend_rate, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = NULL } },
    { MP_QSTR_bend_depth, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = NULL } },
    { MP_QSTR_bend_mode, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = (mp_obj_t)MP_ROM_PTR(&bend_mode_VIBRATO_obj) } },
    { MP_QSTR_waveform, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE } },
    { MP_QSTR_envelope, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE } },
    { MP_QSTR_ring_frequency, MP_ARG_OBJ, {.u_obj = NULL } },
    { MP_QSTR_ring_waveform, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE } },
};
//| class Note:
//|     def __init__(
//|         self,
//|         *,
//|         frequency: float,
//|         panning: float = 0.0,
//|         waveform: Optional[ReadableBuffer] = None,
//|         envelope: Optional[Envelope] = None,
//|         tremolo_depth: float = 0.0,
//|         tremolo_rate: float = 0.0,
//|         bend_depth: float = 0.0,
//|         bend_rate: float = 0.0,
//|         bend_mode: BendMode = BendMode.VIBRATO,
//|     ) -> None:
//|         """Construct a Note object, with a frequency in Hz, and optional panning, waveform, envelope, tremolo (volume change) and bend (frequency change).
//|
//|         If waveform or envelope are `None` the synthesizer object's default waveform or envelope are used.
//|
//|         If the same Note object is played on multiple Synthesizer objects, the result is undefined.
//|         """
STATIC mp_obj_t synthio_note_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(note_properties)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(note_properties), note_properties, args);

    synthio_note_obj_t *self = m_new_obj(synthio_note_obj_t);
    self->base.type = &synthio_note_type;

    mp_obj_t result = MP_OBJ_FROM_PTR(self);
    for (size_t i = 0; i < MP_ARRAY_SIZE(note_properties); i++) {
        if (args[i].u_obj != NULL) {
            mp_store_attr(result, note_properties[i].qst, args[i].u_obj);
        }
    }

    return result;
};

//|     frequency: float
//|     """The base frequency of the note, in Hz."""
STATIC mp_obj_t synthio_note_get_frequency(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_note_get_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_frequency_obj, synthio_note_get_frequency);

STATIC mp_obj_t synthio_note_set_frequency(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_frequency(self, mp_obj_get_float(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_frequency_obj, synthio_note_set_frequency);
MP_PROPERTY_GETSET(synthio_note_frequency_obj,
    (mp_obj_t)&synthio_note_get_frequency_obj,
    (mp_obj_t)&synthio_note_set_frequency_obj);

//|     panning: float
//|     """Defines the channel(s) in which the note appears.
//|
//|     -1 is left channel only, 0 is both channels, and 1 is right channel.
//|     For fractional values, the note plays at full amplitude in one channel
//|     and partial amplitude in the other channel. For instance -.5 plays at full
//|     amplitude in the left channel and 1/2 amplitude in the right channel."""
STATIC mp_obj_t synthio_note_get_panning(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_note_get_panning(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_panning_obj, synthio_note_get_panning);

STATIC mp_obj_t synthio_note_set_panning(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_panning(self, mp_obj_get_float(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_panning_obj, synthio_note_set_panning);
MP_PROPERTY_GETSET(synthio_note_panning_obj,
    (mp_obj_t)&synthio_note_get_panning_obj,
    (mp_obj_t)&synthio_note_set_panning_obj);


//|     tremolo_depth: float
//|     """The tremolo depth of the note, from 0 to 1
//|
//|     A depth of 0 disables tremolo. A nonzero value enables tremolo,
//|     with the maximum decrease in amplitude being equal to the tremolo
//|     depth. A note with a tremolo depth of 1 will fade out to nothing, while
//|     a tremolo depth of 0.1 will give a minimum amplitude of 0.9."""
STATIC mp_obj_t synthio_note_get_tremolo_depth(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_note_get_tremolo_depth(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_tremolo_depth_obj, synthio_note_get_tremolo_depth);

STATIC mp_obj_t synthio_note_set_tremolo_depth(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_tremolo_depth(self, mp_obj_get_float(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_tremolo_depth_obj, synthio_note_set_tremolo_depth);
MP_PROPERTY_GETSET(synthio_note_tremolo_depth_obj,
    (mp_obj_t)&synthio_note_get_tremolo_depth_obj,
    (mp_obj_t)&synthio_note_set_tremolo_depth_obj);

//|     tremolo_rate: float
//|     """The tremolo rate of the note, in Hz."""
STATIC mp_obj_t synthio_note_get_tremolo_rate(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_note_get_tremolo_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_tremolo_rate_obj, synthio_note_get_tremolo_rate);

STATIC mp_obj_t synthio_note_set_tremolo_rate(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_tremolo_rate(self, mp_obj_get_float(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_tremolo_rate_obj, synthio_note_set_tremolo_rate);
MP_PROPERTY_GETSET(synthio_note_tremolo_rate_obj,
    (mp_obj_t)&synthio_note_get_tremolo_rate_obj,
    (mp_obj_t)&synthio_note_set_tremolo_rate_obj);

//|
//|     bend_mode: BendMode
//|     """The type of bend operation"""
STATIC mp_obj_t synthio_note_get_bend_mode(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return cp_enum_find(&synthio_bend_mode_type, common_hal_synthio_note_get_bend_mode(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_bend_mode_obj, synthio_note_get_bend_mode);

STATIC mp_obj_t synthio_note_set_bend_mode(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_bend_mode(self, cp_enum_value(&synthio_bend_mode_type, arg, MP_QSTR_bend_mode));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_bend_mode_obj, synthio_note_set_bend_mode);
MP_PROPERTY_GETSET(synthio_note_bend_mode_obj,
    (mp_obj_t)&synthio_note_get_bend_mode_obj,
    (mp_obj_t)&synthio_note_set_bend_mode_obj);

//
//|
//|     bend_depth: float
//|     """The bend depth of the note, from -1 to +1
//|
//|     A depth of 0 disables bend. A depth of 1 corresponds to a bend of 1
//|     octave.  A depth of (1/12) = 0.833 corresponds to a bend of 1 semitone,
//|     and a depth of .00833 corresponds to one musical cent.
//|     """
STATIC mp_obj_t synthio_note_get_bend_depth(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_note_get_bend_depth(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_bend_depth_obj, synthio_note_get_bend_depth);

STATIC mp_obj_t synthio_note_set_bend_depth(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_bend_depth(self, mp_obj_get_float(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_bend_depth_obj, synthio_note_set_bend_depth);
MP_PROPERTY_GETSET(synthio_note_bend_depth_obj,
    (mp_obj_t)&synthio_note_get_bend_depth_obj,
    (mp_obj_t)&synthio_note_set_bend_depth_obj);

//|     bend_rate: float
//|     """The bend rate of the note, in Hz."""
STATIC mp_obj_t synthio_note_get_bend_rate(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_note_get_bend_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_bend_rate_obj, synthio_note_get_bend_rate);

STATIC mp_obj_t synthio_note_set_bend_rate(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_bend_rate(self, mp_obj_get_float(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_bend_rate_obj, synthio_note_set_bend_rate);
MP_PROPERTY_GETSET(synthio_note_bend_rate_obj,
    (mp_obj_t)&synthio_note_get_bend_rate_obj,
    (mp_obj_t)&synthio_note_set_bend_rate_obj);

//|     waveform: Optional[ReadableBuffer]
//|     """The waveform of this note. Setting the waveform to a buffer of a different size resets the note's phase."""
STATIC mp_obj_t synthio_note_get_waveform(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_note_get_waveform_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_waveform_obj, synthio_note_get_waveform);

STATIC mp_obj_t synthio_note_set_waveform(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_waveform(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_waveform_obj, synthio_note_set_waveform);
MP_PROPERTY_GETSET(synthio_note_waveform_obj,
    (mp_obj_t)&synthio_note_get_waveform_obj,
    (mp_obj_t)&synthio_note_set_waveform_obj);


//|     envelope: Envelope
//|     """The envelope of this note"""
//|
STATIC mp_obj_t synthio_note_get_envelope(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_note_get_envelope_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_envelope_obj, synthio_note_get_envelope);

STATIC mp_obj_t synthio_note_set_envelope(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_envelope(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_envelope_obj, synthio_note_set_envelope);
MP_PROPERTY_GETSET(synthio_note_envelope_obj,
    (mp_obj_t)&synthio_note_get_envelope_obj,
    (mp_obj_t)&synthio_note_set_envelope_obj);

//|     ring_frequency: float
//|     """The ring frequency of the note, in Hz. Zero disables.
//|
//|     For ring to take effect, both ring_frequency and ring_wavefor must be set."""
STATIC mp_obj_t synthio_note_get_ring_frequency(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_synthio_note_get_ring_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_ring_frequency_obj, synthio_note_get_ring_frequency);

STATIC mp_obj_t synthio_note_set_ring_frequency(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_ring_frequency(self, mp_obj_get_float(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_ring_frequency_obj, synthio_note_set_ring_frequency);
MP_PROPERTY_GETSET(synthio_note_ring_frequency_obj,
    (mp_obj_t)&synthio_note_get_ring_frequency_obj,
    (mp_obj_t)&synthio_note_set_ring_frequency_obj);

//|     ring_waveform: Optional[ReadableBuffer]
//|     """The ring waveform of this note. Setting the ring_waveform to a buffer of a different size resets the note's phase.
//|
//|     For ring to take effect, both ring_frequency and ring_wavefor must be set."""
//|
STATIC mp_obj_t synthio_note_get_ring_waveform(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_note_get_ring_waveform_obj(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_ring_waveform_obj, synthio_note_get_ring_waveform);

STATIC mp_obj_t synthio_note_set_ring_waveform(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_ring_waveform(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_ring_waveform_obj, synthio_note_set_ring_waveform);
MP_PROPERTY_GETSET(synthio_note_ring_waveform_obj,
    (mp_obj_t)&synthio_note_get_ring_waveform_obj,
    (mp_obj_t)&synthio_note_set_ring_waveform_obj);



static void note_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    properties_print_helper(print, self_in, note_properties, MP_ARRAY_SIZE(note_properties));
}

STATIC const mp_rom_map_elem_t synthio_note_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&synthio_note_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_panning), MP_ROM_PTR(&synthio_note_panning_obj) },
    { MP_ROM_QSTR(MP_QSTR_waveform), MP_ROM_PTR(&synthio_note_waveform_obj) },
    { MP_ROM_QSTR(MP_QSTR_envelope), MP_ROM_PTR(&synthio_note_envelope_obj) },
    { MP_ROM_QSTR(MP_QSTR_tremolo_depth), MP_ROM_PTR(&synthio_note_tremolo_depth_obj) },
    { MP_ROM_QSTR(MP_QSTR_tremolo_rate), MP_ROM_PTR(&synthio_note_tremolo_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_bend_depth), MP_ROM_PTR(&synthio_note_bend_depth_obj) },
    { MP_ROM_QSTR(MP_QSTR_bend_rate), MP_ROM_PTR(&synthio_note_bend_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_bend_mode), MP_ROM_PTR(&synthio_note_bend_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_ring_frequency), MP_ROM_PTR(&synthio_note_ring_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_ring_waveform), MP_ROM_PTR(&synthio_note_ring_waveform_obj) },
};
STATIC MP_DEFINE_CONST_DICT(synthio_note_locals_dict, synthio_note_locals_dict_table);

const mp_obj_type_t synthio_note_type = {
    { &mp_type_type },
    .name = MP_QSTR_Note,
    .make_new = synthio_note_make_new,
    .locals_dict = (mp_obj_dict_t *)&synthio_note_locals_dict,
    .print = note_print,
};
