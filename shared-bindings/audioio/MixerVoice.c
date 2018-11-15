/*
 * MixerVoice.c
 *
 *  Created on: Nov 15, 2018
 *      Author: dean
 */
#include "shared-bindings/audioio/Mixer.h"
#include "shared-bindings/audioio/MixerVoice.h"

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
// TODO: support mono or stereo voices
STATIC mp_obj_t audioio_mixervoice_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
	audioio_mixervoice_obj_t *self = m_new(audioio_mixervoice_obj_t, 1);
    self->base.type = &audioio_mixervoice_type;

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: deinit()
//|
//|      Deinitialises the Voice and releases any hardware resources for reuse.
//|
STATIC mp_obj_t audioio_mixervoice_deinit(mp_obj_t self_in) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audioio_mixervoice_deinit_obj, audioio_mixervoice_deinit);

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
STATIC mp_obj_t audioio_mixervoice_obj___exit__(size_t n_args, const mp_obj_t *args) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audioio_mixervoice___exit___obj, 4, 4, audioio_mixervoice_obj___exit__);


//|   .. method:: play(sample, *, loop=False)
//|
//|     Plays the sample once when loop=False and continuously when loop=True.
//|     Does not block. Use `playing` to block.
//|
//|     Sample must be an `audioio.WaveFile`, `audioio.Mixer` or `audioio.RawSample`.
//|
//|     The sample must match the Mixer's encoding settings given in the constructor.
//|
STATIC mp_obj_t audioio_mixervoice_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
#if 0
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audioio_mixervoice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    raise_error_if_deinited(common_hal_audioio_mixervoice_deinited(self));
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audioio_mixer_play(self, sample, self->u_int, args[ARG_loop].u_ool);
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audioio_mixervoice_play_obj, 1, audioio_mixervoice_obj_play);

//|   .. method:: stop_voice()
//|
//|     Stops playback of the sample on this voice.
//|
STATIC mp_obj_t audioio_mixervoice_obj_stop(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
#if 0
    enum { ARG_voice };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_voice, MP_ARG_INT, {.u_int = 0} },
    };
    audioio_mixervoice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    raise_error_if_deinited(common_hal_audioio_mixer_deinited(self));
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_audioio_mixer_stop_voice(self, args[ARG_voice].u_int);
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audioio_mixervoice_stop_obj, 1, audioio_mixervoice_obj_stop);

//|   .. method:: set_gain(voice, gain)
//|
//|     Set the gain of a voice.
//|
//|     gain must be a floating point number between 0 and 1
//|
STATIC mp_obj_t audioio_mixervoice_obj_set_gain(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_gain };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_gain,     MP_ARG_OBJ | MP_ARG_REQUIRED },
    };
    audioio_mixervoice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	#if MICROPY_PY_BUILTINS_FLOAT
	float gain = mp_obj_get_float(args[ARG_gain].u_obj);
	#else
	#error "floating point not supported"
	#endif

    if (gain > 1 || gain < 0) {
        mp_raise_ValueError(translate("gain must be between 0 and 1"));
    }

    common_hal_audioio_mixervoice_set_gain(self, gain);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audioio_mixervoice_set_gain_obj, 1, audioio_mixervoice_obj_set_gain);

const mp_obj_property_t audioio_mixervoice_gain_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&audioio_mixervoice_set_gain_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: playing
//|
//|     True when any voice is being output. (read-only)
//|
STATIC mp_obj_t audioio_mixervoice_obj_get_playing(mp_obj_t self_in) {
#if 0
    audioio_mixer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_mixer_deinited(self));
    return mp_obj_new_bool(common_hal_audioio_mixer_get_playing(self));
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_mixervoice_get_playing_obj, audioio_mixervoice_obj_get_playing);

const mp_obj_property_t audioio_mixervoice_playing_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audioio_mixervoice_get_playing_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t audioio_mixervoice_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioio_mixervoice_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audioio_mixervoice___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audioio_mixervoice_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audioio_mixervoice_stop_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audioio_mixervoice_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_gain), MP_ROM_PTR(&audioio_mixervoice_gain_obj) },
};
STATIC MP_DEFINE_CONST_DICT(audioio_mixervoice_locals_dict, audioio_mixervoice_locals_dict_table);

const mp_obj_type_t audioio_mixervoice_type = {
    { &mp_type_type },
    .name = MP_QSTR_mixervoice,
    .make_new = audioio_mixervoice_make_new,
    .locals_dict = (mp_obj_dict_t*)&audioio_mixervoice_locals_dict,
};

