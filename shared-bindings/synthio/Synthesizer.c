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

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-bindings/synthio/Synthesizer.h"
#include "shared-bindings/synthio/__init__.h"
#include "supervisor/shared/translate/translate.h"

//| class Synthesizer:
//|     def __init__(
//|         self,
//|         *,
//|         sample_rate: int = 11025,
//|         waveform: Optional[ReadableBuffer] = None,
//|         envelope: Optional[Envelope] = None,
//|     ) -> None:
//|         """Create a synthesizer object.
//|
//|         This API is experimental.
//|
//|         Notes use MIDI note numbering, with 60 being C4 or Middle C, approximately 262Hz.
//|
//|         :param int sample_rate: The desired playback sample rate; higher sample rate requires more memory
//|         :param ReadableBuffer waveform: A single-cycle waveform. Default is a 50% duty cycle square wave. If specified, must be a ReadableBuffer of type 'h' (signed 16 bit)
//|         :param Optional[Envelope] envelope: An object that defines the loudness of a note over time. The default envelope, `None` provides no ramping, voices turn instantly on and off.
//|         """
STATIC mp_obj_t synthio_synthesizer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_sample_rate, ARG_waveform, ARG_envelope };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 11025} },
        { MP_QSTR_waveform, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_envelope, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo_waveform;
    synthio_synth_parse_waveform(&bufinfo_waveform, args[ARG_waveform].u_obj);

    synthio_synthesizer_obj_t *self = m_new_obj(synthio_synthesizer_obj_t);
    self->base.type = &synthio_synthesizer_type;

    common_hal_synthio_synthesizer_construct(self,
        args[ARG_sample_rate].u_int,
        bufinfo_waveform.buf,
        bufinfo_waveform.len / 2,
        args[ARG_envelope].u_obj);


    return MP_OBJ_FROM_PTR(self);
}

STATIC void check_for_deinit(synthio_synthesizer_obj_t *self) {
    if (common_hal_synthio_synthesizer_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def press(self, /, press: Sequence[int] = ()) -> None:
//|         """Turn some notes on. Notes use MIDI numbering, with 60 being middle C, approximately 262Hz.
//|
//|         Pressing a note that was already pressed has no effect.
//|
//|         :param Sequence[int] press: Any sequence of integer notes."""
STATIC mp_obj_t synthio_synthesizer_press(mp_obj_t self_in, mp_obj_t press) {
    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_synthio_synthesizer_press(self, press);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(synthio_synthesizer_press_obj, synthio_synthesizer_press);
//|     def release(self, /, release: Sequence[int] = ()) -> None:
//|         """Turn some notes off. Notes use MIDI numbering, with 60 being middle C, approximately 262Hz.
//|
//|         Releasing a note that was already released has no effect.
//|
//|         :param Sequence[int] release: Any sequence of integer notes."""
STATIC mp_obj_t synthio_synthesizer_release(mp_obj_t self_in, mp_obj_t release) {
    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_synthio_synthesizer_release(self, release);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(synthio_synthesizer_release_obj, synthio_synthesizer_release);

//|     def release_then_press(
//|         self, release: Sequence[int] = (), press: Sequence[int] = ()
//|     ) -> None:
//|         """Turn some notes on and/or off. Notes use MIDI numbering, with 60 being middle C.
//|
//|         It is OK to release note that was not actually turned on.
//|
//|         Pressing a note that was already pressed has no effect.
//|
//|         Releasing and pressing the note again has little effect, but does reset the phase
//|         of the note, which may be perceptible as a small glitch.
//|
//|         :param Sequence[int] release: Any sequence of integer notes.
//|         :param Sequence[int] press: Any sequence of integer notes."""
STATIC mp_obj_t synthio_synthesizer_release_then_press(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_release, ARG_press };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_release, MP_ARG_OBJ, {.u_obj = mp_const_empty_tuple } },
        { MP_QSTR_press, MP_ARG_OBJ, {.u_obj = mp_const_empty_tuple } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    common_hal_synthio_synthesizer_release(self, args[ARG_release].u_obj);
    common_hal_synthio_synthesizer_press(self, args[ARG_press].u_obj);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(synthio_synthesizer_release_then_press_obj, 1, synthio_synthesizer_release_then_press);

//
//|     def release_all_then_press(self, /, press: Sequence[int]) -> None:
//|         """Turn any currently-playing notes off, then turn on the given notes
//|
//|         Releasing and pressing the note again has little effect, but does reset the phase
//|         of the note, which may be perceptible as a small glitch.
//|
//|         :param Sequence[int] press: Any sequence of integer notes."""
STATIC mp_obj_t synthio_synthesizer_release_all_then_press(mp_obj_t self_in, mp_obj_t press) {
    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_synthio_synthesizer_release_all(self);
    common_hal_synthio_synthesizer_press(self, press);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(synthio_synthesizer_release_all_then_press_obj, synthio_synthesizer_release_all_then_press);

//
//|     def release_all(self) -> None:
//|         """Turn any currently-playing notes off"""
STATIC mp_obj_t synthio_synthesizer_release_all(mp_obj_t self_in) {
    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_synthio_synthesizer_release_all(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(synthio_synthesizer_release_all_obj, synthio_synthesizer_release_all);

//|     def deinit(self) -> None:
//|         """Deinitialises the object and releases any memory resources for reuse."""
//|         ...
STATIC mp_obj_t synthio_synthesizer_deinit(mp_obj_t self_in) {
    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_synthesizer_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(synthio_synthesizer_deinit_obj, synthio_synthesizer_deinit);

//|     def __enter__(self) -> MidiTrack:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.
//|
//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t synthio_synthesizer_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_synthio_synthesizer_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(synthio_synthesizer___exit___obj, 4, 4, synthio_synthesizer_obj___exit__);

//|     envelope: Optional[Envelope]
//|     """The envelope to apply to all notes. `None`, the default envelope, instantly turns notes on and off. The envelope may be changed dynamically, but it affects all notes (even currently playing notes)"""
STATIC mp_obj_t synthio_synthesizer_obj_get_envelope(mp_obj_t self_in) {
    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return synthio_synth_envelope_get(&self->synth);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_synthesizer_get_envelope_obj, synthio_synthesizer_obj_get_envelope);

STATIC mp_obj_t synthio_synthesizer_obj_set_envelope(mp_obj_t self_in, mp_obj_t envelope) {
    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    synthio_synth_envelope_set(&self->synth, envelope);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_synthesizer_set_envelope_obj, synthio_synthesizer_obj_set_envelope);

MP_PROPERTY_GETSET(synthio_synthesizer_envelope_obj,
    (mp_obj_t)&synthio_synthesizer_get_envelope_obj,
    (mp_obj_t)&synthio_synthesizer_set_envelope_obj);

//|     sample_rate: int
//|     """32 bit value that tells how quickly samples are played in Hertz (cycles per second)."""
STATIC mp_obj_t synthio_synthesizer_obj_get_sample_rate(mp_obj_t self_in) {
    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_synthio_synthesizer_get_sample_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_synthesizer_get_sample_rate_obj, synthio_synthesizer_obj_get_sample_rate);

MP_PROPERTY_GETTER(synthio_synthesizer_sample_rate_obj,
    (mp_obj_t)&synthio_synthesizer_get_sample_rate_obj);

//|     pressed: Tuple[int]
//|     """A sequence of the currently pressed notes (read-only property)"""
//|
STATIC mp_obj_t synthio_synthesizer_obj_get_pressed(mp_obj_t self_in) {
    synthio_synthesizer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return common_hal_synthio_synthesizer_get_pressed_notes(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_synthesizer_get_pressed_obj, synthio_synthesizer_obj_get_pressed);

MP_PROPERTY_GETTER(synthio_synthesizer_pressed_obj,
    (mp_obj_t)&synthio_synthesizer_get_pressed_obj);

//|     max_polyphony: int
//|     """Maximum polyphony of the synthesizer (read-only class property)"""
//|

STATIC const mp_rom_map_elem_t synthio_synthesizer_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_press), MP_ROM_PTR(&synthio_synthesizer_press_obj) },
    { MP_ROM_QSTR(MP_QSTR_release), MP_ROM_PTR(&synthio_synthesizer_release_obj) },
    { MP_ROM_QSTR(MP_QSTR_release_all), MP_ROM_PTR(&synthio_synthesizer_release_all_obj) },
    { MP_ROM_QSTR(MP_QSTR_release_then_press), MP_ROM_PTR(&synthio_synthesizer_release_then_press_obj) },
    { MP_ROM_QSTR(MP_QSTR_release_all_then_press), MP_ROM_PTR(&synthio_synthesizer_release_all_then_press_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&synthio_synthesizer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&synthio_synthesizer___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_envelope), MP_ROM_PTR(&synthio_synthesizer_envelope_obj) },
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&synthio_synthesizer_sample_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_max_polyphony), MP_ROM_INT(CIRCUITPY_SYNTHIO_MAX_CHANNELS) },
    { MP_ROM_QSTR(MP_QSTR_pressed), MP_ROM_PTR(&synthio_synthesizer_pressed_obj) },
};
STATIC MP_DEFINE_CONST_DICT(synthio_synthesizer_locals_dict, synthio_synthesizer_locals_dict_table);

STATIC const audiosample_p_t synthio_synthesizer_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .sample_rate = (audiosample_sample_rate_fun)common_hal_synthio_synthesizer_get_sample_rate,
    .bits_per_sample = (audiosample_bits_per_sample_fun)common_hal_synthio_synthesizer_get_bits_per_sample,
    .channel_count = (audiosample_channel_count_fun)common_hal_synthio_synthesizer_get_channel_count,
    .reset_buffer = (audiosample_reset_buffer_fun)synthio_synthesizer_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)synthio_synthesizer_get_buffer,
    .get_buffer_structure = (audiosample_get_buffer_structure_fun)synthio_synthesizer_get_buffer_structure,
};

const mp_obj_type_t synthio_synthesizer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Synthesizer,
    .flags = MP_TYPE_FLAG_EXTENDED,
    .make_new = synthio_synthesizer_make_new,
    .locals_dict = (mp_obj_dict_t *)&synthio_synthesizer_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &synthio_synthesizer_proto,
        ),
};
