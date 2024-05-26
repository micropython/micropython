// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/audiocore/WaveFile.h"
#include "shared-bindings/util.h"
#include "extmod/vfs_posix.h"

//| class WaveFile:
//|     """Load a wave file for audio playback
//|
//|     A .wav file prepped for audio playback. Only mono and stereo files are supported. Samples must
//|     be 8 bit unsigned or 16 bit signed. If a buffer is provided, it will be used instead of allocating
//|     an internal buffer, which can prevent memory fragmentation."""
//|
//|     def __init__(self, file: Union[str, typing.BinaryIO], buffer: WriteableBuffer) -> None:
//|         """Load a .wav file for playback with `audioio.AudioOut` or `audiobusio.I2SOut`.
//|
//|         :param Union[str, typing.BinaryIO] file: The name of a wave file (preferred) or an already opened wave file
//|         :param ~circuitpython_typing.WriteableBuffer buffer: Optional pre-allocated buffer,
//|           that will be split in half and used for double-buffering of the data.
//|           The buffer must be 8 to 1024 bytes long.
//|           If not provided, two 256 byte buffers are initially allocated internally.
//|
//|         Playing a wave file from flash::
//|
//|           import board
//|           import audiocore
//|           import audioio
//|           import digitalio
//|
//|           # Required for CircuitPlayground Express
//|           speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
//|           speaker_enable.switch_to_output(value=True)
//|
//|           wav = audiocore.WaveFile("cplay-5.1-16bit-16khz.wav")
//|           a = audioio.AudioOut(board.A0)
//|
//|           print("playing")
//|           a.play(wav)
//|           while a.playing:
//|             pass
//|           print("stopped")
//|         """
//|         ...
static mp_obj_t audioio_wavefile_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    mp_obj_t arg = args[0];

    if (mp_obj_is_str(arg)) {
        arg = mp_call_function_2(MP_OBJ_FROM_PTR(&mp_builtin_open_obj), arg, MP_ROM_QSTR(MP_QSTR_rb));
    }

    audioio_wavefile_obj_t *self = mp_obj_malloc(audioio_wavefile_obj_t, &audioio_wavefile_type);
    if (!mp_obj_is_type(arg, &mp_type_vfs_fat_fileio)) {
        mp_raise_TypeError(MP_ERROR_TEXT("file must be a file opened in byte mode"));
    }
    uint8_t *buffer = NULL;
    size_t buffer_size = 0;
    if (n_args >= 2) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
        buffer = bufinfo.buf;
        buffer_size = mp_arg_validate_length_range(bufinfo.len, 8, 1024, MP_QSTR_buffer);
    }
    common_hal_audioio_wavefile_construct(self, MP_OBJ_TO_PTR(arg),
        buffer, buffer_size);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the WaveFile and releases all memory resources for reuse."""
//|         ...
static mp_obj_t audioio_wavefile_deinit(mp_obj_t self_in) {
    audioio_wavefile_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_wavefile_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audioio_wavefile_deinit_obj, audioio_wavefile_deinit);

static void check_for_deinit(audioio_wavefile_obj_t *self) {
    if (common_hal_audioio_wavefile_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> WaveFile:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
static mp_obj_t audioio_wavefile_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audioio_wavefile_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audioio_wavefile___exit___obj, 4, 4, audioio_wavefile_obj___exit__);

//|     sample_rate: int
//|     """32 bit value that dictates how quickly samples are loaded into the DAC
//|     in Hertz (cycles per second). When the sample is looped, this can change
//|     the pitch output without changing the underlying sample."""
static mp_obj_t audioio_wavefile_obj_get_sample_rate(mp_obj_t self_in) {
    audioio_wavefile_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioio_wavefile_get_sample_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_wavefile_get_sample_rate_obj, audioio_wavefile_obj_get_sample_rate);

static mp_obj_t audioio_wavefile_obj_set_sample_rate(mp_obj_t self_in, mp_obj_t sample_rate) {
    audioio_wavefile_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_audioio_wavefile_set_sample_rate(self, mp_obj_get_int(sample_rate));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audioio_wavefile_set_sample_rate_obj, audioio_wavefile_obj_set_sample_rate);

MP_PROPERTY_GETSET(audioio_wavefile_sample_rate_obj,
    (mp_obj_t)&audioio_wavefile_get_sample_rate_obj,
    (mp_obj_t)&audioio_wavefile_set_sample_rate_obj);

//|     bits_per_sample: int
//|     """Bits per sample. (read only)"""
static mp_obj_t audioio_wavefile_obj_get_bits_per_sample(mp_obj_t self_in) {
    audioio_wavefile_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioio_wavefile_get_bits_per_sample(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_wavefile_get_bits_per_sample_obj, audioio_wavefile_obj_get_bits_per_sample);

MP_PROPERTY_GETTER(audioio_wavefile_bits_per_sample_obj,
    (mp_obj_t)&audioio_wavefile_get_bits_per_sample_obj);
//|     channel_count: int
//|     """Number of audio channels. (read only)"""
//|
static mp_obj_t audioio_wavefile_obj_get_channel_count(mp_obj_t self_in) {
    audioio_wavefile_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioio_wavefile_get_channel_count(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_wavefile_get_channel_count_obj, audioio_wavefile_obj_get_channel_count);

MP_PROPERTY_GETTER(audioio_wavefile_channel_count_obj,
    (mp_obj_t)&audioio_wavefile_get_channel_count_obj);


static const mp_rom_map_elem_t audioio_wavefile_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioio_wavefile_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audioio_wavefile___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&audioio_wavefile_sample_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_bits_per_sample), MP_ROM_PTR(&audioio_wavefile_bits_per_sample_obj) },
    { MP_ROM_QSTR(MP_QSTR_channel_count), MP_ROM_PTR(&audioio_wavefile_channel_count_obj) },
};
static MP_DEFINE_CONST_DICT(audioio_wavefile_locals_dict, audioio_wavefile_locals_dict_table);

static const audiosample_p_t audioio_wavefile_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .sample_rate = (audiosample_sample_rate_fun)common_hal_audioio_wavefile_get_sample_rate,
    .bits_per_sample = (audiosample_bits_per_sample_fun)common_hal_audioio_wavefile_get_bits_per_sample,
    .channel_count = (audiosample_channel_count_fun)common_hal_audioio_wavefile_get_channel_count,
    .reset_buffer = (audiosample_reset_buffer_fun)audioio_wavefile_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audioio_wavefile_get_buffer,
    .get_buffer_structure = (audiosample_get_buffer_structure_fun)audioio_wavefile_get_buffer_structure,
};


MP_DEFINE_CONST_OBJ_TYPE(
    audioio_wavefile_type,
    MP_QSTR_WaveFile,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audioio_wavefile_make_new,
    locals_dict, &audioio_wavefile_locals_dict,
    protocol, &audioio_wavefile_proto
    );
