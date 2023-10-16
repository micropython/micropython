/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-bindings/synthio/MidiTrack.h"
#include "shared-bindings/synthio/__init__.h"

//| class MidiTrack:
//|     """Simple MIDI synth"""
//|
//|     def __init__(
//|         self,
//|         buffer: ReadableBuffer,
//|         tempo: int,
//|         *,
//|         sample_rate: int = 11025,
//|         waveform: Optional[ReadableBuffer] = None,
//|         envelope: Optional[Envelope] = None,
//|     ) -> None:
//|         """Create a MidiTrack from the given stream of MIDI events. Only "Note On" and "Note Off" events
//|         are supported; channel numbers and key velocities are ignored. Up to two notes may be on at the
//|         same time.
//|
//|         :param ~circuitpython_typing.ReadableBuffer buffer: Stream of MIDI events, as stored in a MIDI file track chunk
//|         :param int tempo: Tempo of the streamed events, in MIDI ticks per second
//|         :param int sample_rate: The desired playback sample rate; higher sample rate requires more memory
//|         :param ReadableBuffer waveform: A single-cycle waveform. Default is a 50% duty cycle square wave. If specified, must be a ReadableBuffer of type 'h' (signed 16 bit)
//|         :param Envelope envelope: An object that defines the loudness of a note over time. The default envelope provides no ramping, voices turn instantly on and off.
//|
//|         Simple melody::
//|
//|           import audioio
//|           import board
//|           import synthio
//|
//|           dac = audioio.AudioOut(board.SPEAKER)
//|           melody = synthio.MidiTrack(b"\\0\\x90H\\0*\\x80H\\0\\6\\x90J\\0*\\x80J\\0\\6\\x90L\\0*\\x80L\\0\\6\\x90J\\0" +
//|                                      b"*\\x80J\\0\\6\\x90H\\0*\\x80H\\0\\6\\x90J\\0*\\x80J\\0\\6\\x90L\\0T\\x80L\\0" +
//|                                      b"\\x0c\\x90H\\0T\\x80H\\0\\x0c\\x90H\\0T\\x80H\\0", tempo=640)
//|           dac.play(melody)
//|           print("playing")
//|           while dac.playing:
//|             pass
//|           print("stopped")"""
//|         ...
STATIC mp_obj_t synthio_miditrack_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_buffer, ARG_tempo, ARG_sample_rate, ARG_waveform, ARG_envelope };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_tempo, MP_ARG_INT | MP_ARG_REQUIRED, {} },
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 11025} },
        { MP_QSTR_waveform, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_envelope, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    synthio_miditrack_obj_t *self = mp_obj_malloc(synthio_miditrack_obj_t, &synthio_miditrack_type);

    common_hal_synthio_miditrack_construct(self,
        (uint8_t *)bufinfo.buf, bufinfo.len,
        args[ARG_tempo].u_int,
        args[ARG_sample_rate].u_int,
        args[ARG_waveform].u_obj,
        mp_const_none,
        args[ARG_envelope].u_obj
        );

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the MidiTrack and releases any hardware resources for reuse."""
//|         ...
STATIC mp_obj_t synthio_miditrack_deinit(mp_obj_t self_in) {
    synthio_miditrack_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_miditrack_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(synthio_miditrack_deinit_obj, synthio_miditrack_deinit);

STATIC void check_for_deinit(synthio_miditrack_obj_t *self) {
    if (common_hal_synthio_miditrack_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> MidiTrack:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t synthio_miditrack_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_synthio_miditrack_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(synthio_miditrack___exit___obj, 4, 4, synthio_miditrack_obj___exit__);

//|     sample_rate: int
//|     """32 bit value that tells how quickly samples are played in Hertz (cycles per second)."""
//|
STATIC mp_obj_t synthio_miditrack_obj_get_sample_rate(mp_obj_t self_in) {
    synthio_miditrack_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_synthio_miditrack_get_sample_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_miditrack_get_sample_rate_obj, synthio_miditrack_obj_get_sample_rate);

MP_PROPERTY_GETTER(synthio_miditrack_sample_rate_obj,
    (mp_obj_t)&synthio_miditrack_get_sample_rate_obj);

//|     error_location: Optional[int]
//|     """Offset, in bytes within the midi data, of a decoding error"""
//|
STATIC mp_obj_t synthio_miditrack_obj_get_error_location(mp_obj_t self_in) {
    synthio_miditrack_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    mp_int_t location = common_hal_synthio_miditrack_get_error_location(self);
    if (location >= 0) {
        return MP_OBJ_NEW_SMALL_INT(location);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_miditrack_get_error_location_obj, synthio_miditrack_obj_get_error_location);

MP_PROPERTY_GETTER(synthio_miditrack_error_location_obj,
    (mp_obj_t)&synthio_miditrack_get_error_location_obj);

STATIC const mp_rom_map_elem_t synthio_miditrack_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&synthio_miditrack_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&synthio_miditrack___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&synthio_miditrack_sample_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_error_location), MP_ROM_PTR(&synthio_miditrack_error_location_obj) },
};
STATIC MP_DEFINE_CONST_DICT(synthio_miditrack_locals_dict, synthio_miditrack_locals_dict_table);

STATIC const audiosample_p_t synthio_miditrack_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .sample_rate = (audiosample_sample_rate_fun)common_hal_synthio_miditrack_get_sample_rate,
    .bits_per_sample = (audiosample_bits_per_sample_fun)common_hal_synthio_miditrack_get_bits_per_sample,
    .channel_count = (audiosample_channel_count_fun)common_hal_synthio_miditrack_get_channel_count,
    .reset_buffer = (audiosample_reset_buffer_fun)synthio_miditrack_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)synthio_miditrack_get_buffer,
    .get_buffer_structure = (audiosample_get_buffer_structure_fun)synthio_miditrack_get_buffer_structure,
};

MP_DEFINE_CONST_OBJ_TYPE(
    synthio_miditrack_type,
    MP_QSTR_MidiTrack,
    MP_TYPE_FLAG_NONE,
    make_new, synthio_miditrack_make_new,
    locals_dict, &synthio_miditrack_locals_dict,
    protocol, &synthio_miditrack_proto
    );
