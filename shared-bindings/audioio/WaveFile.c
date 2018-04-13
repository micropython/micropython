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

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
// #include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
// #include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audioio/WaveFile.h"
#include "shared-bindings/util.h"

//| .. currentmodule:: audioio
//|
//| :class:`WaveFile` -- Load a wave file for audio playback
//| ========================================================
//|
//| A .wav file prepped for audio playback. Only mono and stereo files are supported. Samples must
//| be 8 bit unsigned or 16 bit signed.
//|
//| .. class:: WaveFile(filename)
//|
//|   Load a .wav file for playback with `audioio.AudioOut` or `audiobusio.I2SOut`.
//|
//|   :param bytes-like file: Already opened wave file
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
//|     data = open("cplay-5.1-16bit-16khz.wav", "rb")
//|     wav = audioio.WaveFile(data)
//|     a = audioio.AudioOut(board.A0)
//|
//|     print("playing")
//|     a.play(wav)
//|     while a.playing:
//|       pass
//|     print("stopped")
//|
STATIC mp_obj_t audioio_wavefile_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, true);

    audioio_wavefile_obj_t *self = m_new_obj(audioio_wavefile_obj_t);
    self->base.type = &audioio_wavefile_type;
    if (MP_OBJ_IS_TYPE(args[0], &fatfs_type_fileio)) {
        common_hal_audioio_wavefile_construct(self, MP_OBJ_TO_PTR(args[0]));
    } else {
        mp_raise_TypeError("file must be a file opened in byte mode");
    }

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: deinit()
//|
//|      Deinitialises the WaveFile and releases all memory resources for reuse.
//|
STATIC mp_obj_t audioio_wavefile_deinit(mp_obj_t self_in) {
    audioio_wavefile_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_wavefile_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audioio_wavefile_deinit_obj, audioio_wavefile_deinit);

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
STATIC mp_obj_t audioio_wavefile_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audioio_wavefile_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audioio_wavefile___exit___obj, 4, 4, audioio_wavefile_obj___exit__);

//|   .. attribute:: sample_rate
//|
//|     32 bit value that dictates how quickly samples are loaded into the DAC
//|     in Hertz (cycles per second). When the sample is looped, this can change
//|     the pitch output without changing the underlying sample.
//|
STATIC mp_obj_t audioio_wavefile_obj_get_sample_rate(mp_obj_t self_in) {
    audioio_wavefile_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_wavefile_deinited(self));
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioio_wavefile_get_sample_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioio_wavefile_get_sample_rate_obj, audioio_wavefile_obj_get_sample_rate);

STATIC mp_obj_t audioio_wavefile_obj_set_sample_rate(mp_obj_t self_in, mp_obj_t sample_rate) {
    audioio_wavefile_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_audioio_wavefile_deinited(self));
    common_hal_audioio_wavefile_set_sample_rate(self, mp_obj_get_int(sample_rate));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audioio_wavefile_set_sample_rate_obj, audioio_wavefile_obj_set_sample_rate);

const mp_obj_property_t audioio_wavefile_sample_rate_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&audioio_wavefile_get_sample_rate_obj,
              (mp_obj_t)&audioio_wavefile_set_sample_rate_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t audioio_wavefile_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioio_wavefile_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audioio_wavefile___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&audioio_wavefile_sample_rate_obj) },
};
STATIC MP_DEFINE_CONST_DICT(audioio_wavefile_locals_dict, audioio_wavefile_locals_dict_table);

const mp_obj_type_t audioio_wavefile_type = {
    { &mp_type_type },
    .name = MP_QSTR_WaveFile,
    .make_new = audioio_wavefile_make_new,
    .locals_dict = (mp_obj_dict_t*)&audioio_wavefile_locals_dict,
};
