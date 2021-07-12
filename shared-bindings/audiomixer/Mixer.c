/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/audiomixer/Mixer.h"
#include "shared-bindings/audiomixer/MixerVoice.h"
#include "shared-module/audiomixer/MixerVoice.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class Mixer:
//|     """Mixes one or more audio samples together into one sample."""
//|
//|     def __init__(self, voice_count: int = 2, buffer_size: int = 1024, channel_count: int = 2, bits_per_sample: int = 16, samples_signed: bool = True, sample_rate: int = 8000) -> None:
//|         """Create a Mixer object that can mix multiple channels with the same sample rate.
//|         Samples are accessed and controlled with the mixer's `audiomixer.MixerVoice` objects.
//|
//|         :param int voice_count: The maximum number of voices to mix
//|         :param int buffer_size: The total size in bytes of the buffers to mix into
//|         :param int channel_count: The number of channels the source samples contain. 1 = mono; 2 = stereo.
//|         :param int bits_per_sample: The bits per sample of the samples being played
//|         :param bool samples_signed: Samples are signed (True) or unsigned (False)
//|         :param int sample_rate: The sample rate to be used for all samples
//|
//|         Playing a wave file from flash::
//|
//|           import board
//|           import audioio
//|           import audiocore
//|           import audiomixer
//|           import digitalio
//|
//|           a = audioio.AudioOut(board.A0)
//|           music = audiocore.WaveFile(open("cplay-5.1-16bit-16khz.wav", "rb"))
//|           drum = audiocore.WaveFile(open("drum.wav", "rb"))
//|           mixer = audiomixer.Mixer(voice_count=2, sample_rate=16000, channel_count=1,
//|                                    bits_per_sample=16, samples_signed=True)
//|
//|           print("playing")
//|           # Have AudioOut play our Mixer source
//|           a.play(mixer)
//|           # Play the first sample voice
//|           mixer.voice[0].play(music)
//|           while mixer.playing:
//|             # Play the second sample voice
//|             mixer.voice[1].play(drum)
//|             time.sleep(1)
//|           print("stopped")"""
//|         ...
//|
STATIC mp_obj_t audiomixer_mixer_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_voice_count, ARG_buffer_size, ARG_channel_count, ARG_bits_per_sample, ARG_samples_signed, ARG_sample_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_voice_count, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 2} },
        { MP_QSTR_buffer_size, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1024} },
        { MP_QSTR_channel_count, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 2} },
        { MP_QSTR_bits_per_sample, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 16} },
        { MP_QSTR_samples_signed, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 8000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t voice_count = args[ARG_voice_count].u_int;
    if (voice_count < 1 || voice_count > 255) {
        mp_raise_ValueError(translate("Invalid voice count"));
    }

    mp_int_t channel_count = args[ARG_channel_count].u_int;
    if (channel_count < 1 || channel_count > 2) {
        mp_raise_ValueError(translate("Invalid channel count"));
    }
    mp_int_t sample_rate = args[ARG_sample_rate].u_int;
    if (sample_rate < 1) {
        mp_raise_ValueError(translate("Sample rate must be positive"));
    }
    mp_int_t bits_per_sample = args[ARG_bits_per_sample].u_int;
    if (bits_per_sample != 8 && bits_per_sample != 16) {
        mp_raise_ValueError(translate("bits_per_sample must be 8 or 16"));
    }
    audiomixer_mixer_obj_t *self = m_new_obj_var(audiomixer_mixer_obj_t, mp_obj_t, voice_count);
    self->base.type = &audiomixer_mixer_type;
    common_hal_audiomixer_mixer_construct(self, voice_count, args[ARG_buffer_size].u_int, bits_per_sample, args[ARG_samples_signed].u_bool, channel_count, sample_rate);

    for (int v = 0; v < voice_count; v++) {
        self->voice[v] = audiomixer_mixervoice_type.make_new(&audiomixer_mixervoice_type, 0, 0, NULL);
        common_hal_audiomixer_mixervoice_set_parent(self->voice[v], self);
    }
    self->voice_tuple = mp_obj_new_tuple(self->voice_count, self->voice);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the Mixer and releases any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t audiomixer_mixer_deinit(mp_obj_t self_in) {
    audiomixer_mixer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiomixer_mixer_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audiomixer_mixer_deinit_obj, audiomixer_mixer_deinit);

STATIC void check_for_deinit(audiomixer_mixer_obj_t *self) {
    if (common_hal_audiomixer_mixer_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> Mixer:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t audiomixer_mixer_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audiomixer_mixer_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audiomixer_mixer___exit___obj, 4, 4, audiomixer_mixer_obj___exit__);

//|     playing: bool
//|     """True when any voice is being output. (read-only)"""
//|
STATIC mp_obj_t audiomixer_mixer_obj_get_playing(mp_obj_t self_in) {
    audiomixer_mixer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiomixer_mixer_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomixer_mixer_get_playing_obj, audiomixer_mixer_obj_get_playing);

const mp_obj_property_t audiomixer_mixer_playing_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audiomixer_mixer_get_playing_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     sample_rate: int
//|     """32 bit value that dictates how quickly samples are played in Hertz (cycles per second)."""
//|
STATIC mp_obj_t audiomixer_mixer_obj_get_sample_rate(mp_obj_t self_in) {
    audiomixer_mixer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audiomixer_mixer_get_sample_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomixer_mixer_get_sample_rate_obj, audiomixer_mixer_obj_get_sample_rate);

const mp_obj_property_t audiomixer_mixer_sample_rate_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audiomixer_mixer_get_sample_rate_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     voice: Tuple[MixerVoice, ...]
//|     """A tuple of the mixer's `audiomixer.MixerVoice` object(s).
//|
//|     .. code-block:: python
//|
//|        >>> mixer.voice
//|        (<MixerVoice>,)"""
STATIC mp_obj_t audiomixer_mixer_obj_get_voice(mp_obj_t self_in) {
    audiomixer_mixer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return self->voice_tuple;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomixer_mixer_get_voice_obj, audiomixer_mixer_obj_get_voice);

const mp_obj_property_t audiomixer_mixer_voice_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audiomixer_mixer_get_voice_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def play(self, sample: _typing.AudioSample, *, voice: int = 0, loop: bool = False) -> None:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         Sample must be an `audiocore.WaveFile`, `audiocore.RawSample`, `audiomixer.Mixer` or `audiomp3.MP3Decoder`.
//|
//|         The sample must match the Mixer's encoding settings given in the constructor."""
//|         ...
//|
STATIC mp_obj_t audiomixer_mixer_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_voice, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_voice,     MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiomixer_mixer_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t v = args[ARG_voice].u_int;
    if (v > (self->voice_count - 1)) {
        mp_raise_ValueError(translate("Invalid voice"));
    }
    audiomixer_mixervoice_obj_t *voice = MP_OBJ_TO_PTR(self->voice[v]);
    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiomixer_mixervoice_play(voice, sample, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiomixer_mixer_play_obj, 1, audiomixer_mixer_obj_play);

//|     def stop_voice(self, voice: int = 0) -> None:
//|         """Stops playback of the sample on the given voice."""
//|         ...
//|
STATIC mp_obj_t audiomixer_mixer_obj_stop_voice(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_voice };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_voice, MP_ARG_INT, {.u_int = 0} },
    };
    audiomixer_mixer_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t v = args[ARG_voice].u_int;
    if (v > (self->voice_count - 1)) {
        mp_raise_ValueError(translate("Invalid voice"));
    }
    audiomixer_mixervoice_obj_t *voice = MP_OBJ_TO_PTR(self->voice[v]);
    common_hal_audiomixer_mixervoice_stop(voice);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiomixer_mixer_stop_voice_obj, 1, audiomixer_mixer_obj_stop_voice);


STATIC const mp_rom_map_elem_t audiomixer_mixer_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiomixer_mixer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audiomixer_mixer___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiomixer_mixer_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_voice), MP_ROM_PTR(&audiomixer_mixer_stop_voice_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiomixer_mixer_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&audiomixer_mixer_sample_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_voice), MP_ROM_PTR(&audiomixer_mixer_voice_obj) }
};
STATIC MP_DEFINE_CONST_DICT(audiomixer_mixer_locals_dict, audiomixer_mixer_locals_dict_table);

STATIC const audiosample_p_t audiomixer_mixer_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .sample_rate = (audiosample_sample_rate_fun)common_hal_audiomixer_mixer_get_sample_rate,
    .bits_per_sample = (audiosample_bits_per_sample_fun)common_hal_audiomixer_mixer_get_bits_per_sample,
    .channel_count = (audiosample_channel_count_fun)common_hal_audiomixer_mixer_get_channel_count,
    .reset_buffer = (audiosample_reset_buffer_fun)audiomixer_mixer_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiomixer_mixer_get_buffer,
    .get_buffer_structure = (audiosample_get_buffer_structure_fun)audiomixer_mixer_get_buffer_structure,
};

const mp_obj_type_t audiomixer_mixer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Mixer,
    .flags = MP_TYPE_FLAG_EXTENDED,
    .make_new = audiomixer_mixer_make_new,
    .locals_dict = (mp_obj_dict_t *)&audiomixer_mixer_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &audiomixer_mixer_proto,
        ),
};
