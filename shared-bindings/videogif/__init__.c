// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/videogif/GifDecoder.h"

//| """Support for playing animated GIF files as video
//|
//| The `videogif` module reads animated GIF files so that a
//| `videocore.VideoBuffer` can play them on a display or into a buffer.
//| """

static const mp_rom_map_elem_t videogif_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_videogif) },
    { MP_ROM_QSTR(MP_QSTR_GifDecoder), MP_ROM_PTR(&videogif_gifdecoder_type) },
};

static MP_DEFINE_CONST_DICT(videogif_module_globals, videogif_module_globals_table);

const mp_obj_module_t videogif_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&videogif_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_videogif, videogif_module);
