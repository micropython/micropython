// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 DeanM for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiomixer/Mixer.h"
#include "shared-bindings/audiomixer/MixerVoice.h"

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"

//| class MixerVoice:
//|     """Voice objects used with Mixer
//|
//|     Used to access and control samples with `audiomixer.Mixer`."""
//|
//|     def __init__(self) -> None:
//|         """MixerVoice instance object(s) created by `audiomixer.Mixer`."""
//|         ...
// TODO: support mono or stereo voices
static mp_obj_t audiomixer_mixervoice_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    audiomixer_mixervoice_obj_t *self = mp_obj_malloc(audiomixer_mixervoice_obj_t, &audiomixer_mixervoice_type);

    common_hal_audiomixer_mixervoice_construct(self);

    return MP_OBJ_FROM_PTR(self);
}

//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> None:
//|         """Plays the sample once when ``loop=False``, and continuously when ``loop=True``.
//|         Does not block. Use `playing` to block.
//|
//|         Sample must be an `audiocore.WaveFile`, `audiocore.RawSample`, `audiomixer.Mixer` or `audiomp3.MP3Decoder`.
//|
//|         The sample must match the `audiomixer.Mixer`'s encoding settings given in the constructor.
//|         """
//|         ...
static mp_obj_t audiomixer_mixervoice_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiomixer_mixervoice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiomixer_mixervoice_play(self, sample, args[ARG_loop].u_bool);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiomixer_mixervoice_play_obj, 1, audiomixer_mixervoice_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback of the sample on this voice."""
//|         ...
static mp_obj_t audiomixer_mixervoice_obj_stop(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_voice };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_voice, MP_ARG_INT, {.u_int = 0} },
    };
    audiomixer_mixervoice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_audiomixer_mixervoice_stop(self);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiomixer_mixervoice_stop_obj, 1, audiomixer_mixervoice_obj_stop);

//|     level: float
//|     """The volume level of a voice, as a floating point number between 0 and 1."""
static mp_obj_t audiomixer_mixervoice_obj_get_level(mp_obj_t self_in) {
    return mp_obj_new_float(common_hal_audiomixer_mixervoice_get_level(self_in));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiomixer_mixervoice_get_level_obj, audiomixer_mixervoice_obj_get_level);

static mp_obj_t audiomixer_mixervoice_obj_set_level(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_level };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_level,     MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
    };
    audiomixer_mixervoice_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t level = mp_obj_get_float(args[ARG_level].u_obj);

    if (level > 1 || level < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("level must be between 0 and 1"));
    }

    common_hal_audiomixer_mixervoice_set_level(self, level);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiomixer_mixervoice_set_level_obj, 1, audiomixer_mixervoice_obj_set_level);

MP_PROPERTY_GETSET(audiomixer_mixervoice_level_obj,
    (mp_obj_t)&audiomixer_mixervoice_get_level_obj,
    (mp_obj_t)&audiomixer_mixervoice_set_level_obj);

//|     playing: bool
//|     """True when this voice is being output. (read-only)"""
//|

static mp_obj_t audiomixer_mixervoice_obj_get_playing(mp_obj_t self_in) {
    audiomixer_mixervoice_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(common_hal_audiomixer_mixervoice_get_playing(self));

}
MP_DEFINE_CONST_FUN_OBJ_1(audiomixer_mixervoice_get_playing_obj, audiomixer_mixervoice_obj_get_playing);

MP_PROPERTY_GETTER(audiomixer_mixervoice_playing_obj,
    (mp_obj_t)&audiomixer_mixervoice_get_playing_obj);

static const mp_rom_map_elem_t audiomixer_mixervoice_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiomixer_mixervoice_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiomixer_mixervoice_stop_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiomixer_mixervoice_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_level), MP_ROM_PTR(&audiomixer_mixervoice_level_obj) },
};
static MP_DEFINE_CONST_DICT(audiomixer_mixervoice_locals_dict, audiomixer_mixervoice_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    audiomixer_mixervoice_type,
    MP_QSTR_MixerVoice,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiomixer_mixervoice_make_new,
    locals_dict, &audiomixer_mixervoice_locals_dict
    );
