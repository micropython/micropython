// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "shared-bindings/audiomp3/MP3Decoder.h"
#include "shared-bindings/util.h"

//| class MP3Decoder:
//|     """Load a mp3 file for audio playback
//|
//|     .. note::
//|
//|         ``MP3Decoder`` uses a lot of contiguous memory, so care should be given to
//|         optimizing memory usage.  More information and recommendations can be found here:
//|         https://learn.adafruit.com/Memory-saving-tips-for-CircuitPython/reducing-memory-fragmentation
//|     """
//|
//|     def __init__(self, file: Union[str, typing.BinaryIO], buffer: WriteableBuffer) -> None:
//|         """Load a .mp3 file for playback with `audioio.AudioOut` or `audiobusio.I2SOut`.
//|
//|         :param Union[str, typing.BinaryIO] file: The name of a mp3 file (preferred) or an already opened mp3 file
//|         :param ~circuitpython_typing.WriteableBuffer buffer: Optional pre-allocated buffer, that will be split and used for buffering the data. The buffer is split into two parts for decoded data and the remainder is used for pre-decoded data. When playing from a socket, a larger buffer can help reduce playback glitches at the expense of increased memory usage.
//|
//|         Playback of mp3 audio is CPU intensive, and the
//|         exact limit depends on many factors such as the particular
//|         microcontroller, SD card or flash performance, network performance, and
//|         other code in use such as displayio. If playback is garbled, skips, or plays as
//|         static, first try using a "simpler" mp3:
//|
//|           * Use constant bit rate (CBR) not VBR or ABR (variable or average bit rate) when encoding your mp3 file
//|           * Use a lower sample rate (e.g., 11.025kHz instead of 48kHz)
//|           * Use a lower bit rate (e.g., 32kbit/s instead of 256kbit/s)
//|
//|         Reduce activity taking place at the same time as
//|         mp3 playback. For instance, only update small portions of a
//|         displayio screen if audio is playing. Disable auto-refresh
//|         and explicitly call refresh.
//|
//|         Playing a mp3 file from flash::
//|
//|           import board
//|           import audiomp3
//|           import audioio
//|           import digitalio
//|
//|           # Required for CircuitPlayground Express
//|           speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
//|           speaker_enable.switch_to_output(value=True)
//|
//|           mp3 = audiomp3.MP3Decoder("cplay-16bit-16khz-64kbps.mp3")
//|           a = audioio.AudioOut(board.A0)
//|
//|           print("playing")
//|           a.play(mp3)
//|           while a.playing:
//|             pass
//|           print("stopped")
//|
//|         It is possible to seek within a file before playing it::
//|
//|             with open("/test.mp3", "rb") as stream:
//|                 stream.seek(128000 * 30 // 8) # Seek about 30s into a 128kbit/s stream
//|                 decoder.file = stream
//|
//|         If the stream is played with ``loop = True``, the loop will start at the beginning.
//|         """
//|         ...

static mp_obj_t audiomp3_mp3file_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    mp_obj_t stream = args[0];

    if (mp_obj_is_str(stream)) {
        stream = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), stream, MP_ROM_QSTR(MP_QSTR_rb));
    }

    audiomp3_mp3file_obj_t *self = m_new_obj_with_finaliser(audiomp3_mp3file_obj_t);
    self->base.type = &audiomp3_mp3file_type;

    const mp_stream_p_t *stream_p = mp_get_stream_raise(stream, MP_STREAM_OP_READ);

    if (stream_p->is_text) {
        mp_raise_TypeError(MP_ERROR_TEXT("file must be a file opened in byte mode"));
    }
    uint8_t *buffer = NULL;
    size_t buffer_size = 0;
    if (n_args >= 2) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
        buffer = bufinfo.buf;
        buffer_size = bufinfo.len;
    }
    common_hal_audiomp3_mp3file_construct(self, stream, buffer, buffer_size);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the MP3 and releases all memory resources for reuse."""
//|         ...
static mp_obj_t audiomp3_mp3file_deinit(mp_obj_t self_in) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiomp3_mp3file_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiomp3_mp3file_deinit_obj, audiomp3_mp3file_deinit);

static void check_for_deinit(audiomp3_mp3file_obj_t *self) {
    if (common_hal_audiomp3_mp3file_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> MP3Decoder:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
static mp_obj_t audiomp3_mp3file_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audiomp3_mp3file_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audiomp3_mp3file___exit___obj, 4, 4, audiomp3_mp3file_obj___exit__);

//|     file: typing.BinaryIO
//|     """File to play back."""
static mp_obj_t audiomp3_mp3file_obj_get_file(mp_obj_t self_in) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return self->stream;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomp3_mp3file_get_file_obj, audiomp3_mp3file_obj_get_file);

static mp_obj_t audiomp3_mp3file_obj_set_file(mp_obj_t self_in, mp_obj_t stream) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    const mp_stream_p_t *stream_p = mp_get_stream_raise(stream, MP_STREAM_OP_READ);

    if (stream_p->is_text) {
        mp_raise_TypeError(MP_ERROR_TEXT("file must be a file opened in byte mode"));
    }
    common_hal_audiomp3_mp3file_set_file(self, stream);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiomp3_mp3file_set_file_obj, audiomp3_mp3file_obj_set_file);

//|     def open(self, filepath: str) -> None:
//|         """Takes in the name of a mp3 file, opens it, and replaces the old playback file."""
//|         ...
static mp_obj_t audiomp3_mp3file_obj_open(mp_obj_t self_in, mp_obj_t path) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    mp_obj_t file = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), path, MP_ROM_QSTR(MP_QSTR_rb));

    common_hal_audiomp3_mp3file_set_file(self, file);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiomp3_mp3file_open_obj, audiomp3_mp3file_obj_open);

MP_PROPERTY_GETSET(audiomp3_mp3file_file_obj,
    (mp_obj_t)&audiomp3_mp3file_get_file_obj,
    (mp_obj_t)&audiomp3_mp3file_set_file_obj);



//|     sample_rate: int
//|     """32 bit value that dictates how quickly samples are loaded into the DAC
//|     in Hertz (cycles per second). When the sample is looped, this can change
//|     the pitch output without changing the underlying sample."""
static mp_obj_t audiomp3_mp3file_obj_get_sample_rate(mp_obj_t self_in) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audiomp3_mp3file_get_sample_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomp3_mp3file_get_sample_rate_obj, audiomp3_mp3file_obj_get_sample_rate);

static mp_obj_t audiomp3_mp3file_obj_set_sample_rate(mp_obj_t self_in, mp_obj_t sample_rate) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_audiomp3_mp3file_set_sample_rate(self, mp_obj_get_int(sample_rate));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiomp3_mp3file_set_sample_rate_obj, audiomp3_mp3file_obj_set_sample_rate);

MP_PROPERTY_GETSET(audiomp3_mp3file_sample_rate_obj,
    (mp_obj_t)&audiomp3_mp3file_get_sample_rate_obj,
    (mp_obj_t)&audiomp3_mp3file_set_sample_rate_obj);

//|     bits_per_sample: int
//|     """Bits per sample. (read only)"""
static mp_obj_t audiomp3_mp3file_obj_get_bits_per_sample(mp_obj_t self_in) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audiomp3_mp3file_get_bits_per_sample(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomp3_mp3file_get_bits_per_sample_obj, audiomp3_mp3file_obj_get_bits_per_sample);

MP_PROPERTY_GETTER(audiomp3_mp3file_bits_per_sample_obj,
    (mp_obj_t)&audiomp3_mp3file_get_bits_per_sample_obj);

//|     channel_count: int
//|     """Number of audio channels. (read only)"""
static mp_obj_t audiomp3_mp3file_obj_get_channel_count(mp_obj_t self_in) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audiomp3_mp3file_get_channel_count(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomp3_mp3file_get_channel_count_obj, audiomp3_mp3file_obj_get_channel_count);

MP_PROPERTY_GETTER(audiomp3_mp3file_channel_count_obj,
    (mp_obj_t)&audiomp3_mp3file_get_channel_count_obj);

//|     rms_level: float
//|     """The RMS audio level of a recently played moment of audio. (read only)"""
static mp_obj_t audiomp3_mp3file_obj_get_rms_level(mp_obj_t self_in) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_audiomp3_mp3file_get_rms_level(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomp3_mp3file_get_rms_level_obj, audiomp3_mp3file_obj_get_rms_level);

MP_PROPERTY_GETTER(audiomp3_mp3file_rms_level_obj,
    (mp_obj_t)&audiomp3_mp3file_get_rms_level_obj);

//|     samples_decoded: int
//|     """The number of audio samples decoded from the current file. (read only)"""
//|
static mp_obj_t audiomp3_mp3file_obj_get_samples_decoded(mp_obj_t self_in) {
    audiomp3_mp3file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audiomp3_mp3file_get_samples_decoded(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomp3_mp3file_get_samples_decoded_obj, audiomp3_mp3file_obj_get_samples_decoded);

MP_PROPERTY_GETTER(audiomp3_mp3file_samples_decoded_obj,
    (mp_obj_t)&audiomp3_mp3file_get_samples_decoded_obj);

static const mp_rom_map_elem_t audiomp3_mp3file_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&audiomp3_mp3file_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiomp3_mp3file_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&audiomp3_mp3file_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audiomp3_mp3file___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_file), MP_ROM_PTR(&audiomp3_mp3file_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&audiomp3_mp3file_sample_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_bits_per_sample), MP_ROM_PTR(&audiomp3_mp3file_bits_per_sample_obj) },
    { MP_ROM_QSTR(MP_QSTR_channel_count), MP_ROM_PTR(&audiomp3_mp3file_channel_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_rms_level), MP_ROM_PTR(&audiomp3_mp3file_rms_level_obj) },
    { MP_ROM_QSTR(MP_QSTR_samples_decoded), MP_ROM_PTR(&audiomp3_mp3file_samples_decoded_obj) },
};
static MP_DEFINE_CONST_DICT(audiomp3_mp3file_locals_dict, audiomp3_mp3file_locals_dict_table);

static const audiosample_p_t audiomp3_mp3file_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .sample_rate = (audiosample_sample_rate_fun)common_hal_audiomp3_mp3file_get_sample_rate,
    .bits_per_sample = (audiosample_bits_per_sample_fun)common_hal_audiomp3_mp3file_get_bits_per_sample,
    .channel_count = (audiosample_channel_count_fun)common_hal_audiomp3_mp3file_get_channel_count,
    .reset_buffer = (audiosample_reset_buffer_fun)audiomp3_mp3file_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiomp3_mp3file_get_buffer,
    .get_buffer_structure = (audiosample_get_buffer_structure_fun)audiomp3_mp3file_get_buffer_structure,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiomp3_mp3file_type,
    MP_QSTR_MP3Decoder,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiomp3_mp3file_make_new,
    locals_dict, &audiomp3_mp3file_locals_dict,
    protocol, &audiomp3_mp3file_proto
    );
