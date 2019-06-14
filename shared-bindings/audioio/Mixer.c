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
#include "shared-bindings/audioio/Mixer.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audioio/RawSample.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| .. currentmodule:: audioio
//|
//| :class:`Mixer` -- Mixes one or more audio samples together
//| ===========================================================
//|
//| Mixer mixes multiple samples into one sample.
//|
//| .. class:: Mixer(channel_count=2, buffer_size=1024)
//|
//|   Create a Mixer object that can mix multiple channels with the same sample rate.
//|
//|   :param int channel_count: The maximum number of samples to mix at once
//|   :param int buffer_size: The total size in bytes of the buffers to mix into
//|
//|   Playing a wave file from flash::
//|
//|     import board
//|     import audioio
//|     import digitalio
//|
//|     # Required for CircuitPlayground Express
//|     speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
//|     speaker_enable.switch_to_output(value=True)
//|
//|     music = audioio.WaveFile(open("cplay-5.1-16bit-16khz.wav", "rb"))
//|     drum = audioio.WaveFile(open("drum.wav", "rb"))
//|     mixer = audioio.Mixer(voice_count=2, sample_rate=16000, channel_count=1, bits_per_sample=16, samples_signed=True)
//|     a = audioio.AudioOut(board.A0)
//|
//|     print("playing")
//|     a.play(mixer)
//|     mixer.play(music, voice=0)
//|     while mixer.playing:
//|       mixer.play(drum, voice=1)
//|       time.sleep(1)
//|     print("stopped")
//|
STATIC mp_obj_t audioio_mixer_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
    audioio_mixer_obj_t *self = m_new_obj_var(audioio_mixer_obj_t, audioio_mixer_voice_t, voice_count);
    self->base.type = &audioio_mixer_type;
    common_hal_audioio_mixer_construct(self, voice_count, args[ARG_buffer_size].u_int, bits_per_sample, args[ARG_samples_signed].u_bool, channel_count, sample_rate);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: deinit()
//|
//|      Deinitialises the Mixer and releases any hardware resources for reuse.
//|
STATIC mp_obj_t audioio_mixer_deinit(mp_obj_t self_in) {
    audioio_mixer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_mixer_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audioio_mixer_deinit_obj, audioio_mixer_deinit);

STATIC void check_for_deinit(audioio_mixer_obj_t *self) {
    if (common_hal_audioio_mixer_deinited(self)) {
        raise_deinited_error();
    }
}

//|   .. method:: __enter__()
//|
//|      No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: __exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context. See
//|      :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t audioio_mixer_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audioio_mixer_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audioio_mixer___exit___obj, 4, 4, audioio_mixer_obj___exit__);


//|   .. method:: play(sample, *, voice=0, loop=False)
//|
//|     Plays the sample once when loop=False and continuously when loop=True.
//|     Does not block. Use `playing` to block.
//|
//|     Sample must be an `audioio.WaveFile`, `audioio.Mixer` or `audioio.RawSample`.
//|
//|     The sample must match the Mixer's encoding settings given in the constructor.
//|
STATIC mp_obj_t audioio_mixer_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_voice, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_voice,     MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audioio_mixer_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audioio_mixer_play(self, sample, args[ARG_voice].u_int, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audioio_mixer_play_obj, 1, audioio_mixer_obj_play);

//|   .. method:: stop_voice(voice=0)
//|
//|     Stops playback of the sample on the given voice.
//|
STATIC mp_obj_t audioio_mixer_obj_stop_voice(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_voice };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_voice, MP_ARG_INT, {.u_int = 0} },
    };
    audioio_mixer_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_audioio_mixer_stop_voice(self, args[ARG_voice].u_int);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audioio_mixer_stop_voice_obj, 1, audioio_mixer_obj_stop_voice);

//|   .. attribute:: playing
//|
//|     True when any voice is being output. (read-only)
//|
STATIC mp_obj_t audioio_mixer_obj_get_playing(mp_obj_t self_in) {
    audioio_mixer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audioio_mixer_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_mixer_get_playing_obj, audioio_mixer_obj_get_playing);

const mp_obj_property_t audioio_mixer_playing_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audioio_mixer_get_playing_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: sample_rate
//|
//|     32 bit value that dictates how quickly samples are played in Hertz (cycles per second).
//|
STATIC mp_obj_t audioio_mixer_obj_get_sample_rate(mp_obj_t self_in) {
    audioio_mixer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioio_mixer_get_sample_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_mixer_get_sample_rate_obj, audioio_mixer_obj_get_sample_rate);


const mp_obj_property_t audioio_mixer_sample_rate_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audioio_mixer_get_sample_rate_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t audioio_mixer_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioio_mixer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audioio_mixer___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audioio_mixer_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_voice), MP_ROM_PTR(&audioio_mixer_stop_voice_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audioio_mixer_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&audioio_mixer_sample_rate_obj) },
};
STATIC MP_DEFINE_CONST_DICT(audioio_mixer_locals_dict, audioio_mixer_locals_dict_table);

const mp_obj_type_t audioio_mixer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Mixer,
    .make_new = audioio_mixer_make_new,
    .locals_dict = (mp_obj_dict_t*)&audioio_mixer_locals_dict,
};
