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
#include "shared-bindings/audiobusio/I2SOut.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class I2SOut:
//|     """Output an I2S audio signal"""
//|
//|     def __init__(self, bit_clock: microcontroller.Pin, word_select: microcontroller.Pin, data: microcontroller.Pin, *, left_justified: bool) -> None:
//|         """Create a I2SOut object associated with the given pins.
//|
//|         :param ~microcontroller.Pin bit_clock: The bit clock (or serial clock) pin
//|         :param ~microcontroller.Pin word_select: The word select (or left/right clock) pin
//|         :param ~microcontroller.Pin data: The data pin
//|         :param bool left_justified: True when data bits are aligned with the word select clock. False
//|           when they are shifted by one to match classic I2S protocol.
//|
//|         Simple 8ksps 440 Hz sine wave on `Metro M0 Express <https://www.adafruit.com/product/3505>`_
//|         using `UDA1334 Breakout <https://www.adafruit.com/product/3678>`_::
//|
//|           import audiobusio
//|           import audiocore
//|           import board
//|           import array
//|           import time
//|           import math
//|
//|           # Generate one period of sine wave.
//|           length = 8000 // 440
//|           sine_wave = array.array("H", [0] * length)
//|           for i in range(length):
//|               sine_wave[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15) + 2 ** 15)
//|
//|           sine_wave = audiocore.RawSample(sine_wave, sample_rate=8000)
//|           i2s = audiobusio.I2SOut(board.D1, board.D0, board.D9)
//|           i2s.play(sine_wave, loop=True)
//|           time.sleep(1)
//|           i2s.stop()
//|
//|         Playing a wave file from flash::
//|
//|           import board
//|           import audioio
//|           import audiocore
//|           import audiobusio
//|           import digitalio
//|
//|
//|           f = open("cplay-5.1-16bit-16khz.wav", "rb")
//|           wav = audiocore.WaveFile(f)
//|
//|           a = audiobusio.I2SOut(board.D1, board.D0, board.D9)
//|
//|           print("playing")
//|           a.play(wav)
//|           while a.playing:
//|             pass
//|           print("stopped")"""
//|         ...
//|
STATIC mp_obj_t audiobusio_i2sout_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    #if !CIRCUITPY_AUDIOBUSIO_I2SOUT
    mp_raise_NotImplementedError(translate("I2SOut not available"));
    return NULL;                // Not reachable.
    #else
    enum { ARG_bit_clock, ARG_word_select, ARG_data, ARG_left_justified };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bit_clock, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_word_select, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_data, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_left_justified, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *bit_clock = validate_obj_is_free_pin(args[ARG_bit_clock].u_obj);
    const mcu_pin_obj_t *word_select = validate_obj_is_free_pin(args[ARG_word_select].u_obj);
    const mcu_pin_obj_t *data = validate_obj_is_free_pin(args[ARG_data].u_obj);

    audiobusio_i2sout_obj_t *self = m_new_obj_with_finaliser(audiobusio_i2sout_obj_t);
    self->base.type = &audiobusio_i2sout_type;
    common_hal_audiobusio_i2sout_construct(self, bit_clock, word_select, data, args[ARG_left_justified].u_bool);

    return MP_OBJ_FROM_PTR(self);
    #endif
}

#if CIRCUITPY_AUDIOBUSIO_I2SOUT

//|     def deinit(self) -> None:
//|         """Deinitialises the I2SOut and releases any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t audiobusio_i2sout_deinit(mp_obj_t self_in) {
    audiobusio_i2sout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiobusio_i2sout_deinit(self);
    return mp_const_none;

}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2sout_deinit_obj, audiobusio_i2sout_deinit);

STATIC void check_for_deinit(audiobusio_i2sout_obj_t *self) {
    if (common_hal_audiobusio_i2sout_deinited(self)) {
        raise_deinited_error();
    }
}
//|     def __enter__(self) -> I2SOut:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t audiobusio_i2sout_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audiobusio_i2sout_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audiobusio_i2sout___exit___obj, 4, 4, audiobusio_i2sout_obj___exit__);


//|     def play(self, sample: _typing.AudioSample, *, loop: bool = False) -> None:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         Sample must be an `audiocore.WaveFile`, `audiocore.RawSample`, `audiomixer.Mixer` or `audiomp3.MP3Decoder`.
//|
//|         The sample itself should consist of 8 bit or 16 bit samples."""
//|         ...
//|
STATIC mp_obj_t audiobusio_i2sout_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiobusio_i2sout_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiobusio_i2sout_play(self, sample, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiobusio_i2sout_play_obj, 1, audiobusio_i2sout_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback."""
//|         ...
//|
STATIC mp_obj_t audiobusio_i2sout_obj_stop(mp_obj_t self_in) {
    audiobusio_i2sout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_audiobusio_i2sout_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2sout_stop_obj, audiobusio_i2sout_obj_stop);

//|     playing: bool
//|     """True when the audio sample is being output. (read-only)"""
//|
STATIC mp_obj_t audiobusio_i2sout_obj_get_playing(mp_obj_t self_in) {
    audiobusio_i2sout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiobusio_i2sout_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2sout_get_playing_obj, audiobusio_i2sout_obj_get_playing);

const mp_obj_property_t audiobusio_i2sout_playing_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audiobusio_i2sout_get_playing_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def pause(self) -> None:
//|         """Stops playback temporarily while remembering the position. Use `resume` to resume playback."""
//|         ...
//|
STATIC mp_obj_t audiobusio_i2sout_obj_pause(mp_obj_t self_in) {
    audiobusio_i2sout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (!common_hal_audiobusio_i2sout_get_playing(self)) {
        mp_raise_RuntimeError(translate("Not playing"));
    }
    common_hal_audiobusio_i2sout_pause(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2sout_pause_obj, audiobusio_i2sout_obj_pause);

//|     def resume(self) -> None:
//|         """Resumes sample playback after :py:func:`pause`."""
//|         ...
//|
STATIC mp_obj_t audiobusio_i2sout_obj_resume(mp_obj_t self_in) {
    audiobusio_i2sout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (common_hal_audiobusio_i2sout_get_paused(self)) {
        common_hal_audiobusio_i2sout_resume(self);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2sout_resume_obj, audiobusio_i2sout_obj_resume);

//|     paused: bool
//|     """True when playback is paused. (read-only)"""
//|
STATIC mp_obj_t audiobusio_i2sout_obj_get_paused(mp_obj_t self_in) {
    audiobusio_i2sout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiobusio_i2sout_get_paused(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2sout_get_paused_obj, audiobusio_i2sout_obj_get_paused);

const mp_obj_property_t audiobusio_i2sout_paused_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audiobusio_i2sout_get_paused_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};
#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT

STATIC const mp_rom_map_elem_t audiobusio_i2sout_locals_dict_table[] = {
    // Methods
    #if CIRCUITPY_AUDIOBUSIO_I2SOUT
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&audiobusio_i2sout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiobusio_i2sout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audiobusio_i2sout___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiobusio_i2sout_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiobusio_i2sout_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&audiobusio_i2sout_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&audiobusio_i2sout_resume_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiobusio_i2sout_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_paused), MP_ROM_PTR(&audiobusio_i2sout_paused_obj) },
    #endif // CIRCUITPY_AUDIOBUSIO_I2SOUT
};
STATIC MP_DEFINE_CONST_DICT(audiobusio_i2sout_locals_dict, audiobusio_i2sout_locals_dict_table);

const mp_obj_type_t audiobusio_i2sout_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2SOut,
    .make_new = audiobusio_i2sout_make_new,
    .locals_dict = (mp_obj_dict_t *)&audiobusio_i2sout_locals_dict,
};
