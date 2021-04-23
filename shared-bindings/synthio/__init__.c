/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Artyom Skrobov
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

#include <string.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "extmod/vfs_fat.h"

#include "shared-bindings/synthio/__init__.h"
#include "shared-bindings/synthio/MidiTrack.h"

//| """Support for MIDI synthesis"""
//|
//| def from_file(file: typing.BinaryIO, *, sample_rate: int = 11025) -> MidiTrack:
//|     """Create an AudioSample from an already opened MIDI file.
//|     Currently, only single-track MIDI (type 0) is supported.
//|
//|     :param typing.BinaryIO file: Already opened MIDI file
//|     :param int sample_rate: The desired playback sample rate; higher sample rate requires more memory
//|
//|
//|     Playing a MIDI file from flash::
//|
//|           import audioio
//|           import board
//|           import synthio
//|
//|           data = open("single-track.midi", "rb")
//|           midi = synthio.from_file(data)
//|           a = audioio.AudioOut(board.A0)
//|
//|           print("playing")
//|           a.play(midi)
//|           while a.playing:
//|             pass
//|           print("stopped")"""
//|     ...
//|
STATIC mp_obj_t synthio_from_file(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_file, ARG_sample_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 11025} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if (!mp_obj_is_type(args[ARG_file].u_obj, &mp_type_fileio)) {
        mp_raise_TypeError(translate("file must be a file opened in byte mode"));
    }
    pyb_file_obj_t *file = MP_OBJ_TO_PTR(args[ARG_file].u_obj);

    uint8_t chunk_header[14];
    f_rewind(&file->fp);
    UINT bytes_read;
    if (f_read(&file->fp, chunk_header, sizeof(chunk_header), &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != sizeof(chunk_header) ||
        memcmp(chunk_header, "MThd\0\0\0\6\0\0\0\1", 12)) {
        mp_raise_ValueError(translate("Invalid MIDI file"));
        // TODO: for a multi-track MIDI (type 1), return an AudioMixer
    }

    uint16_t tempo;
    if (chunk_header[12] & 0x80) {
        tempo = -(int8_t)chunk_header[12] * chunk_header[13];
    } else {
        tempo = 2 * ((chunk_header[12] << 8) | chunk_header[13]);
    }

    if (f_read(&file->fp, chunk_header, 8, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != 8 || memcmp(chunk_header, "MTrk", 4)) {
        mp_raise_ValueError(translate("Invalid MIDI file"));
    }
    uint32_t track_size = (chunk_header[4] << 24) |
        (chunk_header[5] << 16) | (chunk_header[6] << 8) | chunk_header[7];
    uint8_t *buffer = m_malloc(track_size, false);
    if (f_read(&file->fp, buffer, track_size, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != track_size) {
        mp_raise_ValueError(translate("Invalid MIDI file"));
    }

    synthio_miditrack_obj_t *result = m_new_obj(synthio_miditrack_obj_t);
    result->base.type = &synthio_miditrack_type;

    common_hal_synthio_miditrack_construct(result, buffer, track_size,
        tempo, args[ARG_sample_rate].u_int);

    m_free(buffer);

    return MP_OBJ_FROM_PTR(result);
}
MP_DEFINE_CONST_FUN_OBJ_KW(synthio_from_file_obj, 1, synthio_from_file);


STATIC const mp_rom_map_elem_t synthio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_synthio) },
    { MP_ROM_QSTR(MP_QSTR_MidiTrack), MP_ROM_PTR(&synthio_miditrack_type) },
    { MP_ROM_QSTR(MP_QSTR_from_file), MP_ROM_PTR(&synthio_from_file_obj) },
};

STATIC MP_DEFINE_CONST_DICT(synthio_module_globals, synthio_module_globals_table);

const mp_obj_module_t synthio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&synthio_module_globals,
};
