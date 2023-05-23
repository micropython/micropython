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
    { MP_QSTR_amplitude, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(1) } },
    { MP_QSTR_bend, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(0) } },
    { MP_QSTR_waveform, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE } },
    { MP_QSTR_envelope, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE } },
    { MP_QSTR_filter, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(1) } },
    { MP_QSTR_ring_frequency, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(0) } },
    { MP_QSTR_ring_bend, MP_ARG_OBJ, {.u_obj = MP_ROM_INT(0) } },
    { MP_QSTR_ring_waveform, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE } },
};
//| class Note:
//|     def __init__(
//|         self,
//|         *,
//|         frequency: float,
//|         panning: BlockInput = 0.0,
//|         waveform: Optional[ReadableBuffer] = None,
//|         envelope: Optional[Envelope] = None,
//|         amplitude: BlockInput = 0.0,
//|         bend: BlockInput = 0.0,
//|         ring_frequency: float = 0.0,
//|         ring_bend: float = 0.0,
//|         ring_waveform: Optional[ReadableBuffer] = 0.0,
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
    properties_construct_helper(result, note_properties, args, MP_ARRAY_SIZE(note_properties));

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

//|     filter: bool
//|     """True if the note should be processed via the synthesizer's FIR filter."""
STATIC mp_obj_t synthio_note_get_filter(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_synthio_note_get_filter(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_filter_obj, synthio_note_get_filter);

STATIC mp_obj_t synthio_note_set_filter(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_filter(self, mp_obj_is_true(arg));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_filter_obj, synthio_note_set_filter);
MP_PROPERTY_GETSET(synthio_note_filter_obj,
    (mp_obj_t)&synthio_note_get_filter_obj,
    (mp_obj_t)&synthio_note_set_filter_obj);

//|     panning: BlockInput
//|     """Defines the channel(s) in which the note appears.
//|
//|     -1 is left channel only, 0 is both channels, and 1 is right channel.
//|     For fractional values, the note plays at full amplitude in one channel
//|     and partial amplitude in the other channel. For instance -.5 plays at full
//|     amplitude in the left channel and 1/2 amplitude in the right channel."""
STATIC mp_obj_t synthio_note_get_panning(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_note_get_panning(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_panning_obj, synthio_note_get_panning);

STATIC mp_obj_t synthio_note_set_panning(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_panning(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_panning_obj, synthio_note_set_panning);
MP_PROPERTY_GETSET(synthio_note_panning_obj,
    (mp_obj_t)&synthio_note_get_panning_obj,
    (mp_obj_t)&synthio_note_set_panning_obj);


//|     amplitude: BlockInput
//|     """The relative amplitude of the note, from 0 to 1
//|
//|     An amplitude of 0 makes the note inaudible. It is combined multiplicatively with
//|     the value from the note's envelope.
//|
//|     To achieve a tremolo effect, attach an LFO here."""
STATIC mp_obj_t synthio_note_get_amplitude(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_note_get_amplitude(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_amplitude_obj, synthio_note_get_amplitude);

STATIC mp_obj_t synthio_note_set_amplitude(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_amplitude(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_amplitude_obj, synthio_note_set_amplitude);
MP_PROPERTY_GETSET(synthio_note_amplitude_obj,
    (mp_obj_t)&synthio_note_get_amplitude_obj,
    (mp_obj_t)&synthio_note_set_amplitude_obj);

//|
//|     bend: BlockInput
//|     """The pitch bend depth of the note, from -12 to +12
//|
//|     A depth of 0 plays the programmed frequency. A depth of 1 corresponds to a bend of 1
//|     octave.  A depth of (1/12) = 0.0833 corresponds to a bend of 1 semitone,
//|     and a depth of .00833 corresponds to one musical cent.
//|
//|     To achieve a vibrato or sweep effect, attach an LFO here.
//|     """
STATIC mp_obj_t synthio_note_get_bend(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_note_get_bend(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_bend_obj, synthio_note_get_bend);

STATIC mp_obj_t synthio_note_set_bend(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_bend(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_bend_obj, synthio_note_set_bend);
MP_PROPERTY_GETSET(synthio_note_bend_obj,
    (mp_obj_t)&synthio_note_get_bend_obj,
    (mp_obj_t)&synthio_note_set_bend_obj);

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
//|     For ring to take effect, both ``ring_frequency`` and ``ring_waveform`` must be set."""
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

//|     ring_bend: float
//|     """The pitch bend depth of the note's ring waveform, from -12 to +12
//|
//|     A depth of 0 plays the programmed frequency. A depth of 1 corresponds to a bend of 1
//|     octave.  A depth of (1/12) = 0.0833 corresponds to a bend of 1 semitone,
//|     and a depth of .00833 corresponds to one musical cent.
//|
//|     To achieve a vibrato or sweep effect on the ring waveform, attach an LFO here.
//|     """
STATIC mp_obj_t synthio_note_get_ring_bend(mp_obj_t self_in) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_note_get_ring_bend(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_note_get_ring_bend_obj, synthio_note_get_ring_bend);

STATIC mp_obj_t synthio_note_set_ring_bend(mp_obj_t self_in, mp_obj_t arg) {
    synthio_note_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_note_set_ring_bend(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_note_set_ring_bend_obj, synthio_note_set_ring_bend);
MP_PROPERTY_GETSET(synthio_note_ring_bend_obj,
    (mp_obj_t)&synthio_note_get_ring_bend_obj,
    (mp_obj_t)&synthio_note_set_ring_bend_obj);

//|     ring_waveform: Optional[ReadableBuffer]
//|     """The ring waveform of this note. Setting the ring_waveform to a buffer of a different size resets the note's phase.
//|
//|     For ring to take effect, both ``ring_frequency`` and ``ring_waveform`` must be set."""
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
    { MP_ROM_QSTR(MP_QSTR_filter), MP_ROM_PTR(&synthio_note_filter_obj) },
    { MP_ROM_QSTR(MP_QSTR_panning), MP_ROM_PTR(&synthio_note_panning_obj) },
    { MP_ROM_QSTR(MP_QSTR_waveform), MP_ROM_PTR(&synthio_note_waveform_obj) },
    { MP_ROM_QSTR(MP_QSTR_envelope), MP_ROM_PTR(&synthio_note_envelope_obj) },
    { MP_ROM_QSTR(MP_QSTR_amplitude), MP_ROM_PTR(&synthio_note_amplitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_bend), MP_ROM_PTR(&synthio_note_bend_obj) },
    { MP_ROM_QSTR(MP_QSTR_ring_frequency), MP_ROM_PTR(&synthio_note_ring_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_ring_bend), MP_ROM_PTR(&synthio_note_ring_bend_obj) },
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
