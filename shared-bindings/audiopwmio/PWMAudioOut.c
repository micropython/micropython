/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audiopwmio/PWMAudioOut.h"
#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class PWMAudioOut:
//|     """Output an analog audio signal by varying the PWM duty cycle."""
//|
//|     def __init__(self, left_channel: microcontroller.Pin, *, right_channel: Optional[microcontroller.Pin] = None, quiescent_value: int = 0x8000) -> None:
//|         """Create a PWMAudioOut object associated with the given pin(s). This allows you to
//|         play audio signals out on the given pin(s).  In contrast to mod:`audioio`,
//|         the pin(s) specified are digital pins, and are driven with a device-dependent PWM
//|         signal.
//|
//|         :param ~microcontroller.Pin left_channel: The pin to output the left channel to
//|         :param ~microcontroller.Pin right_channel: The pin to output the right channel to
//|         :param int quiescent_value: The output value when no signal is present. Samples should start
//|             and end with this value to prevent audible popping.
//|
//|         Simple 8ksps 440 Hz sin wave::
//|
//|           import audiocore
//|           import audiopwmio
//|           import board
//|           import array
//|           import time
//|           import math
//|
//|           # Generate one period of sine wav.
//|           length = 8000 // 440
//|           sine_wave = array.array("H", [0] * length)
//|           for i in range(length):
//|               sine_wave[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15) + 2 ** 15)
//|
//|           dac = audiopwmio.PWMAudioOut(board.SPEAKER)
//|           sine_wave = audiocore.RawSample(sine_wave, sample_rate=8000)
//|           dac.play(sine_wave, loop=True)
//|           time.sleep(1)
//|           dac.stop()
//|
//|         Playing a wave file from flash::
//|
//|           import board
//|           import audiocore
//|           import audiopwmio
//|           import digitalio
//|
//|           # Required for CircuitPlayground Express
//|           speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
//|           speaker_enable.switch_to_output(value=True)
//|
//|           data = open("cplay-5.1-16bit-16khz.wav", "rb")
//|           wav = audiocore.WaveFile(data)
//|           a = audiopwmio.PWMAudioOut(board.SPEAKER)
//|
//|           print("playing")
//|           a.play(wav)
//|           while a.playing:
//|             pass
//|           print("stopped")"""
//|         ...
//|
STATIC mp_obj_t audiopwmio_pwmaudioout_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_left_channel, ARG_right_channel, ARG_quiescent_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_left_channel, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_right_channel, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_quiescent_value, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x8000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *left_channel_pin = validate_obj_is_free_pin(args[ARG_left_channel].u_obj);
    const mcu_pin_obj_t *right_channel_pin = validate_obj_is_free_pin_or_none(args[ARG_right_channel].u_obj);

    // create AudioOut object from the given pin
    audiopwmio_pwmaudioout_obj_t *self = m_new_obj(audiopwmio_pwmaudioout_obj_t);
    self->base.type = &audiopwmio_pwmaudioout_type;
    common_hal_audiopwmio_pwmaudioout_construct(self, left_channel_pin, right_channel_pin, args[ARG_quiescent_value].u_int);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the PWMAudioOut and releases any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t audiopwmio_pwmaudioout_deinit(mp_obj_t self_in) {
    audiopwmio_pwmaudioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiopwmio_pwmaudioout_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audiopwmio_pwmaudioout_deinit_obj, audiopwmio_pwmaudioout_deinit);

STATIC void check_for_deinit(audiopwmio_pwmaudioout_obj_t *self) {
    if (common_hal_audiopwmio_pwmaudioout_deinited(self)) {
        raise_deinited_error();
    }
}
//|     def __enter__(self) -> PWMAudioOut:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t audiopwmio_pwmaudioout_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audiopwmio_pwmaudioout_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audiopwmio_pwmaudioout___exit___obj, 4, 4, audiopwmio_pwmaudioout_obj___exit__);


//|     def play(self, sample: _typing.AudioSample, *, loop: bool = False) -> None:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         Sample must be an `audiocore.WaveFile`, `audiocore.RawSample`, `audiomixer.Mixer` or `audiomp3.MP3Decoder`.
//|
//|         The sample itself should consist of 16 bit samples. Microcontrollers with a lower output
//|         resolution will use the highest order bits to output."""
//|         ...
//|
STATIC mp_obj_t audiopwmio_pwmaudioout_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiopwmio_pwmaudioout_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiopwmio_pwmaudioout_play(self, sample, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiopwmio_pwmaudioout_play_obj, 1, audiopwmio_pwmaudioout_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback and resets to the start of the sample."""
//|         ...
//|
STATIC mp_obj_t audiopwmio_pwmaudioout_obj_stop(mp_obj_t self_in) {
    audiopwmio_pwmaudioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_audiopwmio_pwmaudioout_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiopwmio_pwmaudioout_stop_obj, audiopwmio_pwmaudioout_obj_stop);

//|     playing: bool
//|     """True when an audio sample is being output even if `paused`. (read-only)"""
//|
STATIC mp_obj_t audiopwmio_pwmaudioout_obj_get_playing(mp_obj_t self_in) {
    audiopwmio_pwmaudioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiopwmio_pwmaudioout_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiopwmio_pwmaudioout_get_playing_obj, audiopwmio_pwmaudioout_obj_get_playing);

const mp_obj_property_t audiopwmio_pwmaudioout_playing_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audiopwmio_pwmaudioout_get_playing_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def pause(self) -> None:
//|         """Stops playback temporarily while remembering the position. Use `resume` to resume playback."""
//|         ...
//|
STATIC mp_obj_t audiopwmio_pwmaudioout_obj_pause(mp_obj_t self_in) {
    audiopwmio_pwmaudioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (!common_hal_audiopwmio_pwmaudioout_get_playing(self)) {
        mp_raise_RuntimeError(translate("Not playing"));
    }
    common_hal_audiopwmio_pwmaudioout_pause(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiopwmio_pwmaudioout_pause_obj, audiopwmio_pwmaudioout_obj_pause);

//|     def resume(self) -> None:
//|         """Resumes sample playback after :py:func:`pause`."""
//|         ...
//|
STATIC mp_obj_t audiopwmio_pwmaudioout_obj_resume(mp_obj_t self_in) {
    audiopwmio_pwmaudioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (common_hal_audiopwmio_pwmaudioout_get_paused(self)) {
        common_hal_audiopwmio_pwmaudioout_resume(self);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiopwmio_pwmaudioout_resume_obj, audiopwmio_pwmaudioout_obj_resume);

//|     paused: bool
//|     """True when playback is paused. (read-only)"""
//|
STATIC mp_obj_t audiopwmio_pwmaudioout_obj_get_paused(mp_obj_t self_in) {
    audiopwmio_pwmaudioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiopwmio_pwmaudioout_get_paused(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiopwmio_pwmaudioout_get_paused_obj, audiopwmio_pwmaudioout_obj_get_paused);

const mp_obj_property_t audiopwmio_pwmaudioout_paused_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audiopwmio_pwmaudioout_get_paused_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t audiopwmio_pwmaudioout_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiopwmio_pwmaudioout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audiopwmio_pwmaudioout___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiopwmio_pwmaudioout_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiopwmio_pwmaudioout_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&audiopwmio_pwmaudioout_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&audiopwmio_pwmaudioout_resume_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiopwmio_pwmaudioout_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_paused), MP_ROM_PTR(&audiopwmio_pwmaudioout_paused_obj) },
};
STATIC MP_DEFINE_CONST_DICT(audiopwmio_pwmaudioout_locals_dict, audiopwmio_pwmaudioout_locals_dict_table);

const mp_obj_type_t audiopwmio_pwmaudioout_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWMAudioOut,
    .make_new = audiopwmio_pwmaudioout_make_new,
    .locals_dict = (mp_obj_dict_t *)&audiopwmio_pwmaudioout_locals_dict,
};
