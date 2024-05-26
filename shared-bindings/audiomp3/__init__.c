// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/audiomp3/MP3Decoder.h"

//| """Support for MP3-compressed audio files
//|
//| For more information about working with MP3 files in CircuitPython,
//| see `this CircuitPython Essentials Learn guide page
//| <https://learn.adafruit.com/circuitpython-essentials/circuitpython-mp3-audio>`_.
//| """

static const mp_rom_map_elem_t audiomp3_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiomp3) },
    { MP_ROM_QSTR(MP_QSTR_MP3Decoder), MP_ROM_PTR(&audiomp3_mp3file_type) },
};

static MP_DEFINE_CONST_DICT(audiomp3_module_globals, audiomp3_module_globals_table);

const mp_obj_module_t audiomp3_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiomp3_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audiomp3, audiomp3_module);
