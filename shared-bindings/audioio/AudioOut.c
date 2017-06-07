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
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audioio/AudioOut.h"

//| .. currentmodule:: audioio
//|
//| :class:`AudioOut` -- Output an analog audio signal
//| ========================================================
//|
//| AudioOut can be used to output an analog audio signal on a given pin.
//|
//| .. class:: AudioOut(pin, sample_source)
//|
//|   Create a AudioOut object associated with the given pin. This allows you to
//|   play audio signals out on the given pin. Sample_source must be a `bytes-like object <https://docs.python.org/3/glossary.html#term-bytes-like-object>`_.
//|
//|   The sample itself should consist of 16 bit samples and be mono.
//|   Microcontrollers with a lower output resolution will use the highest order
//|   bits to output. For example, the SAMD21 has a 10 bit DAC that ignores the
//|   lowest 6 bits when playing 16 bit samples.
//|
//|   :param ~microcontroller.Pin pin: The pin to output to
//|   :param bytes-like sample_source: The source of the sample
//|
//|   Simple 8ksps 440 Hz sin wave::
//|
//|     import audioio
//|     import board
//|     import array
//|     import time
//|     import math
//|
//|     # Generate one period of sine wav.
//|     length = 8000 // 440
//|     b = array.array("H", [0] * length)
//|     for i in range(length):
//|         b[i] = int(math.sin(math.pi * 2 * i / 18) * (2 ** 15) + 2 ** 15)
//|
//|     sample = audioio.AudioOut(board.SPEAKER, sin_wave)
//|     sample.play(loop=True)
//|     time.sleep(1)
//|     sample.stop()
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
//|     f = open("cplay-5.1-16bit-16khz.wav", "rb")
//
//|     a = audioio.AudioOut(board.A0, f)
//|
//|     print("playing")
//|     a.play()
//|     while a.playing:
//|       pass
//|     print("stopped")
//|
STATIC mp_obj_t audioio_audioout_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, true);
    mp_obj_t pin_obj = args[0];
    assert_pin(pin_obj, false);
    const mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(pin_obj);
    // We explicitly don't check whether the pin is free because multiple
    // AudioOuts may share it.

    // create PWM object from the given pin
    audioio_audioout_obj_t *self = m_new_obj(audioio_audioout_obj_t);
    self->base.type = &audioio_audioout_type;
    mp_buffer_info_t bufinfo;
    if (MP_OBJ_IS_TYPE(args[1], &fatfs_type_fileio)) {
        common_hal_audioio_audioout_construct_from_file(self, pin, MP_OBJ_TO_PTR(args[1]));
    } else if (mp_get_buffer(args[1], &bufinfo, MP_BUFFER_READ)) {
        if (bufinfo.len % 2 == 1) {
            mp_raise_ValueError("sample_source must be an even number of bytes (two per sample)");
        }
        common_hal_audioio_audioout_construct_from_buffer(self, pin, ((uint16_t*)bufinfo.buf), bufinfo.len / 2);
    } else {
        mp_raise_TypeError("sample_source must be a file or bytes-like object.");
    }

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: deinit()
//|
//|      Deinitialises the PWMOut and releases any hardware resources for reuse.
//|
STATIC mp_obj_t audioio_audioout_deinit(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_audioout_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_deinit_obj, audioio_audioout_deinit);

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
STATIC mp_obj_t audioio_audioout_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audioio_audioout_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audioio_audioout___exit___obj, 4, 4, audioio_audioout_obj___exit__);


//|   .. method:: play(loop=False)
//|
//|     Plays the sample once when loop=False and continuously when loop=True.
//|     Does not block. Use `playing` to block.
//|
STATIC mp_obj_t audioio_audioout_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_loop,      MP_ARG_BOOL, {.u_bool = false} },
    };
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_audioio_audioout_play(self, args[ARG_loop].u_bool);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audioio_audioout_play_obj, 1, audioio_audioout_obj_play);

//|   .. method:: stop()
//|
//|     Stops playback of this sample. If another sample is playing instead, it
//|     won't be stopped.
//|
STATIC mp_obj_t audioio_audioout_obj_stop(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_audioout_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_stop_obj, audioio_audioout_obj_stop);

//|   .. attribute:: playing
//|
//|     True when the audio sample is being output.
//|
STATIC mp_obj_t audioio_audioout_obj_get_playing(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_audioio_audioout_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_get_playing_obj, audioio_audioout_obj_get_playing);

const mp_obj_property_t audioio_audioout_playing_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audioio_audioout_get_playing_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: frequency
//|
//|     32 bit value that dictates how quickly samples are loaded into the DAC
//|     in Hertz (cycles per second). When the sample is looped, this can change
//|     the pitch output without changing the underlying sample.
//|
STATIC mp_obj_t audioio_audioout_obj_get_frequency(mp_obj_t self_in) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioio_audioout_get_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioout_get_frequency_obj, audioio_audioout_obj_get_frequency);

STATIC mp_obj_t audioio_audioout_obj_set_frequency(mp_obj_t self_in, mp_obj_t frequency) {
    audioio_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_audioout_set_frequency(self, mp_obj_get_int(frequency));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audioio_audioout_set_frequency_obj, audioio_audioout_obj_set_frequency);

const mp_obj_property_t audioio_audioout_frequency_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audioio_audioout_get_frequency_obj,
              (mp_obj_t)&audioio_audioout_set_frequency_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t audioio_audioout_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioio_audioout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audioio_audioout___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audioio_audioout_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audioio_audioout_stop_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audioio_audioout_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&audioio_audioout_frequency_obj) },
};
STATIC MP_DEFINE_CONST_DICT(audioio_audioout_locals_dict, audioio_audioout_locals_dict_table);

const mp_obj_type_t audioio_audioout_type = {
    { &mp_type_type },
    .name = MP_QSTR_AudioOut,
    .make_new = audioio_audioout_make_new,
    .locals_dict = (mp_obj_dict_t*)&audioio_audioout_locals_dict,
};
