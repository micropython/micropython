// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Michael Schroeder
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/audiomixer/Mixer.h"

//| """Support for audio mixing"""

static const mp_rom_map_elem_t audiomixer_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiomixer) },
    { MP_ROM_QSTR(MP_QSTR_Mixer), MP_ROM_PTR(&audiomixer_mixer_type) },
};

static MP_DEFINE_CONST_DICT(audiomixer_module_globals, audiomixer_module_globals_table);

const mp_obj_module_t audiomixer_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiomixer_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audiomixer, audiomixer_module);
