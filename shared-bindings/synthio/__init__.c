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
#include "extmod/vfs_fat.h"
#include "extmod/vfs_posix.h"

#include "shared-bindings/synthio/__init__.h"
#include "shared-bindings/synthio/Biquad.h"
#include "shared-bindings/synthio/LFO.h"
#include "shared-bindings/synthio/Math.h"
#include "shared-bindings/synthio/MidiTrack.h"
#include "shared-bindings/synthio/Note.h"
#include "shared-bindings/synthio/Synthesizer.h"

#include "shared-module/synthio/LFO.h"

//|
//| """Support for multi-channel audio synthesis
//|
//| At least 2 simultaneous notes are supported.  samd5x, mimxrt10xx and rp2040 platforms support up to 12 notes.
//| """
//|

//| class EnvelopeState:
//|     ATTACK: EnvelopeState
//|     """The note is in its attack phase"""
//|     DECAY: EnvelopeState
//|     """The note is in its decay phase"""
//|     SUSTAIN: EnvelopeState
//|     """The note is in its sustain phase"""
//|     RELEASE: EnvelopeState
//|     """The note is in its release phase"""
//|
MAKE_ENUM_VALUE(synthio_note_state_type, note_state, ATTACK, SYNTHIO_ENVELOPE_STATE_ATTACK);
MAKE_ENUM_VALUE(synthio_note_state_type, note_state, DECAY, SYNTHIO_ENVELOPE_STATE_DECAY);
MAKE_ENUM_VALUE(synthio_note_state_type, note_state, SUSTAIN, SYNTHIO_ENVELOPE_STATE_SUSTAIN);
MAKE_ENUM_VALUE(synthio_note_state_type, note_state, RELEASE, SYNTHIO_ENVELOPE_STATE_RELEASE);

MAKE_ENUM_MAP(synthio_note_state) {
    MAKE_ENUM_MAP_ENTRY(note_state, ATTACK),
    MAKE_ENUM_MAP_ENTRY(note_state, DECAY),
    MAKE_ENUM_MAP_ENTRY(note_state, SUSTAIN),
    MAKE_ENUM_MAP_ENTRY(note_state, RELEASE),
};

STATIC MP_DEFINE_CONST_DICT(synthio_note_state_locals_dict, synthio_note_state_locals_table);
MAKE_PRINTER(synthio, synthio_note_state);
MAKE_ENUM_TYPE(synthio, EnvelopeState, synthio_note_state);

#define default_attack_time (MICROPY_FLOAT_CONST(0.1))
#define default_decay_time (MICROPY_FLOAT_CONST(0.05))
#define default_release_time (MICROPY_FLOAT_CONST(0.2))
#define default_attack_level (MICROPY_FLOAT_CONST(1.))
#define default_sustain_level (MICROPY_FLOAT_CONST(0.8))

static const mp_arg_t envelope_properties[] = {
    { MP_QSTR_attack_time, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL } },
    { MP_QSTR_decay_time, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL } },
    { MP_QSTR_release_time, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL } },
    { MP_QSTR_attack_level, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL } },
    { MP_QSTR_sustain_level, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL } },
};

//| BlockInput = Union["Math", "LFO", float, None]
//| """Blocks and Notes can take any of these types as inputs on certain attributes
//|
//| A BlockInput can be any of the following types: `Math`, `LFO`, `builtins.float`, `None` (treated same as 0).
//| """
//|
//| class Envelope:
//|     def __init__(
//|         self,
//|         *,
//|         attack_time: Optional[float] = 0.1,
//|         decay_time: Optional[float] = 0.05,
//|         release_time: Optional[float] = 0.2,
//|         attack_level: Optional[float] = 1.0,
//|         sustain_level: Optional[float] = 0.8,
//|     ) -> None:
//|         """Construct an Envelope object
//|
//|         The Envelope defines an ADSR (Attack, Decay, Sustain, Release) envelope with linear amplitude ramping. A note starts at 0 volume, then increases to ``attack_level`` over ``attack_time`` seconds; then it decays to ``sustain_level`` over ``decay_time`` seconds. Finally, when the note is released, it decreases to ``0`` volume over ``release_time``.
//|
//|         If the ``sustain_level`` of an envelope is 0, then the decay and sustain phases of the note are always omitted. The note is considered to be released as soon as the envelope reaches the end of the attack phase. The ``decay_time`` is ignored. This is similar to how a plucked or struck instrument behaves.
//|
//|         If a note is released before it reaches its sustain phase, it decays with the same slope indicated by ``sustain_level/release_time`` (or ``attack_level/release_time`` for plucked envelopes)
//|
//|         :param float attack_time: The time in seconds it takes to ramp from 0 volume to attack_volume
//|         :param float decay_time: The time in seconds it takes to ramp from attack_volume to sustain_volume
//|         :param float release_time: The time in seconds it takes to ramp from sustain_volume to release_volume. When a note is released before it has reached the sustain phase, the release is done with the same slope indicated by ``release_time`` and ``sustain_level``. If the ``sustain_level`` is ``0.0`` then the release slope calculations use the ``attack_level`` instead.
//|         :param float attack_level: The level, in the range ``0.0`` to ``1.0`` of the peak volume of the attack phase
//|         :param float sustain_level: The level, in the range ``0.0`` to ``1.0`` of the volume of the sustain phase relative to the attack level
//|         """
//|     attack_time: float
//|     """The time in seconds it takes to ramp from 0 volume to attack_volume"""
//|
//|     decay_time: float
//|     """The time in seconds it takes to ramp from attack_volume to sustain_volume"""
//|
//|     release_time: float
//|     """The time in seconds it takes to ramp from sustain_volume to release_volume. When a note is released before it has reached the sustain phase, the release is done with the same slope indicated by ``release_time`` and ``sustain_level``"""
//|
//|     attack_level: float
//|     """The level, in the range ``0.0`` to ``1.0`` of the peak volume of the attack phase"""
//|
//|     sustain_level: float
//|     """The level, in the range ``0.0`` to ``1.0`` of the volume of the sustain phase relative to the attack level"""
//|

STATIC mp_obj_t synthio_envelope_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(envelope_properties)];
    enum { ARG_attack_time, ARG_decay_time, ARG_release_time, ARG_attack_level, ARG_sustain_level };
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(envelope_properties), envelope_properties, args);

    if (args[ARG_attack_time].u_obj == MP_OBJ_NULL) {
        args[ARG_attack_time].u_obj = mp_obj_new_float(default_attack_time);
    }
    if (args[ARG_decay_time].u_obj == MP_OBJ_NULL) {
        args[ARG_decay_time].u_obj = mp_obj_new_float(default_decay_time);
    }
    if (args[ARG_release_time].u_obj == MP_OBJ_NULL) {
        args[ARG_release_time].u_obj = mp_obj_new_float(default_release_time);
    }
    if (args[ARG_attack_level].u_obj == MP_OBJ_NULL) {
        args[ARG_attack_level].u_obj = mp_obj_new_float(default_attack_level);
    }
    if (args[ARG_sustain_level].u_obj == MP_OBJ_NULL) {
        args[ARG_sustain_level].u_obj = mp_obj_new_float(default_sustain_level);
    }

    mp_arg_validate_obj_float_non_negative(args[ARG_attack_time].u_obj, 0., MP_QSTR_attack_time);
    mp_arg_validate_obj_float_non_negative(args[ARG_decay_time].u_obj, 0., MP_QSTR_decay_time);
    mp_arg_validate_obj_float_non_negative(args[ARG_release_time].u_obj, 0., MP_QSTR_release_time);

    mp_arg_validate_obj_float_range(args[ARG_attack_level].u_obj, 0, 1, MP_QSTR_attack_level);
    mp_arg_validate_obj_float_range(args[ARG_sustain_level].u_obj, 0, 1, MP_QSTR_sustain_level);

    MP_STATIC_ASSERT(sizeof(mp_arg_val_t) == sizeof(mp_obj_t));
    return namedtuple_make_new(type_in, MP_ARRAY_SIZE(args), 0, &args[0].u_obj);
};

const mp_obj_namedtuple_type_t synthio_envelope_type_obj = {
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS_MAKE_NEW(MP_QSTR_Envelope, synthio_envelope_make_new),
    .n_fields = 5,
    .fields = {
        MP_QSTR_attack_time,
        MP_QSTR_decay_time,
        MP_QSTR_release_time,
        MP_QSTR_attack_level,
        MP_QSTR_sustain_level,
    },
};

//| def from_file(
//|     file: typing.BinaryIO,
//|     *,
//|     sample_rate: int = 11025,
//|     waveform: Optional[ReadableBuffer] = None,
//|     envelope: Optional[Envelope] = None,
//| ) -> MidiTrack:
//|     """Create an AudioSample from an already opened MIDI file.
//|     Currently, only single-track MIDI (type 0) is supported.
//|
//|     :param typing.BinaryIO file: Already opened MIDI file
//|     :param int sample_rate: The desired playback sample rate; higher sample rate requires more memory
//|     :param ReadableBuffer waveform: A single-cycle waveform. Default is a 50% duty cycle square wave. If specified, must be a ReadableBuffer of type 'h' (signed 16 bit)
//|     :param Envelope envelope: An object that defines the loudness of a note over time. The default envelope provides no ramping, voices turn instantly on and off.
//|
//|     Playing a MIDI file from flash::
//|
//|           import audioio
//|           import board
//|           import synthio
//|
//|           data = open("single-track.midi", "rb")
//|           midi = synthio.from_file(data)
//|           a = audioio.AudioOut(board.A0)
//|
//|           print("playing")
//|           a.play(midi)
//|           while a.playing:
//|             pass
//|           print("stopped")"""
//|     ...
//|
STATIC mp_obj_t synthio_from_file(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_file, ARG_sample_rate, ARG_waveform, ARG_envelope };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 11025} },
        { MP_QSTR_waveform, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_envelope, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if (!mp_obj_is_type(args[ARG_file].u_obj, &mp_type_vfs_fat_fileio)) {
        mp_raise_TypeError(translate("file must be a file opened in byte mode"));
    }
    pyb_file_obj_t *file = MP_OBJ_TO_PTR(args[ARG_file].u_obj);

    uint8_t chunk_header[14];
    f_rewind(&file->fp);
    UINT bytes_read;
    if (f_read(&file->fp, chunk_header, sizeof(chunk_header), &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != sizeof(chunk_header) ||
        memcmp(chunk_header, "MThd\0\0\0\6\0\0\0\1", 12)) {
        mp_arg_error_invalid(MP_QSTR_file);
        // TODO: for a multi-track MIDI (type 1), return an AudioMixer
    }

    uint16_t tempo;
    if (chunk_header[12] & 0x80) {
        tempo = -(int8_t)chunk_header[12] * chunk_header[13];
    } else {
        tempo = 2 * ((chunk_header[12] << 8) | chunk_header[13]);
    }

    if (f_read(&file->fp, chunk_header, 8, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != 8 || memcmp(chunk_header, "MTrk", 4)) {
        mp_arg_error_invalid(MP_QSTR_file);
    }
    uint32_t track_size = (chunk_header[4] << 24) |
        (chunk_header[5] << 16) | (chunk_header[6] << 8) | chunk_header[7];
    uint8_t *buffer = m_malloc(track_size);
    if (f_read(&file->fp, buffer, track_size, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != track_size) {
        mp_arg_error_invalid(MP_QSTR_file);
    }

    synthio_miditrack_obj_t *result = mp_obj_malloc(synthio_miditrack_obj_t, &synthio_miditrack_type);

    common_hal_synthio_miditrack_construct(result, buffer, track_size,
        tempo, args[ARG_sample_rate].u_int, args[ARG_waveform].u_obj,
        mp_const_none,
        args[ARG_envelope].u_obj
        );

    #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    m_free(buffer, track_size);
    #else
    m_free(buffer);
    #endif

    return MP_OBJ_FROM_PTR(result);
}
MP_DEFINE_CONST_FUN_OBJ_KW(synthio_from_file_obj, 1, synthio_from_file);

//| def midi_to_hz(midi_note: float) -> float:
//|     """Converts the given midi note (60 = middle C, 69 = concert A) to Hz"""
//|

STATIC mp_obj_t midi_to_hz(mp_obj_t arg) {
    mp_float_t note = mp_arg_validate_type_float(arg, MP_QSTR_note);
    return mp_obj_new_float(common_hal_synthio_midi_to_hz_float(note));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_midi_to_hz_obj, midi_to_hz);

//| def voct_to_hz(ctrl: float) -> float:
//|     """Converts a 1v/octave signal to Hz.
//|
//|     24/12 (2.0) corresponds to middle C, 33/12 (2.75) is concert A."""
//|

STATIC mp_obj_t voct_to_hz(mp_obj_t arg) {
    mp_float_t note = mp_arg_validate_obj_float_range(arg, -11, 11, MP_QSTR_ctrl);
    return mp_obj_new_float(common_hal_synthio_voct_to_hz_float(note));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_voct_to_hz_obj, voct_to_hz);

#if CIRCUITPY_AUDIOCORE_DEBUG
STATIC mp_obj_t synthio_lfo_tick(size_t n, const mp_obj_t *args) {
    shared_bindings_synthio_lfo_tick(48000);
    mp_obj_t result[n];
    for (size_t i = 0; i < n; i++) {
        synthio_block_slot_t slot;
        synthio_block_assign_slot(args[i], &slot, MP_QSTR_arg);
        mp_float_t value = synthio_block_slot_get(&slot);
        result[i] = mp_obj_new_float(value);
    }
    return mp_obj_new_tuple(n, result);
}
MP_DEFINE_CONST_FUN_OBJ_VAR(synthio_lfo_tick_obj, 1, synthio_lfo_tick);
#endif

STATIC const mp_rom_map_elem_t synthio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_synthio) },
    { MP_ROM_QSTR(MP_QSTR_Biquad), MP_ROM_PTR(&synthio_biquad_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_Math), MP_ROM_PTR(&synthio_math_type) },
    { MP_ROM_QSTR(MP_QSTR_MathOperation), MP_ROM_PTR(&synthio_math_operation_type) },
    { MP_ROM_QSTR(MP_QSTR_MidiTrack), MP_ROM_PTR(&synthio_miditrack_type) },
    { MP_ROM_QSTR(MP_QSTR_Note), MP_ROM_PTR(&synthio_note_type) },
    { MP_ROM_QSTR(MP_QSTR_EnvelopeState), MP_ROM_PTR(&synthio_note_state_type) },
    { MP_ROM_QSTR(MP_QSTR_LFO), MP_ROM_PTR(&synthio_lfo_type) },
    { MP_ROM_QSTR(MP_QSTR_Synthesizer), MP_ROM_PTR(&synthio_synthesizer_type) },
    { MP_ROM_QSTR(MP_QSTR_from_file), MP_ROM_PTR(&synthio_from_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_Envelope), MP_ROM_PTR(&synthio_envelope_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_midi_to_hz), MP_ROM_PTR(&synthio_midi_to_hz_obj) },
    { MP_ROM_QSTR(MP_QSTR_voct_to_hz), MP_ROM_PTR(&synthio_voct_to_hz_obj) },
    #if CIRCUITPY_AUDIOCORE_DEBUG
    { MP_ROM_QSTR(MP_QSTR_lfo_tick), MP_ROM_PTR(&synthio_lfo_tick_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(synthio_module_globals, synthio_module_globals_table);

const mp_obj_module_t synthio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&synthio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_synthio, synthio_module);
