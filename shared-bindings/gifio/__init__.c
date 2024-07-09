// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared-bindings/gifio/GifWriter.h"
#include "shared-bindings/gifio/OnDiskGif.h"
#include "shared-bindings/util.h"

//| """Access GIF-format images
//| """
static const mp_rom_map_elem_t gifio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_gifio) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_GifWriter),  MP_ROM_PTR(&gifio_gifwriter_type)},
    { MP_ROM_QSTR(MP_QSTR_OnDiskGif), MP_ROM_PTR(&gifio_ondiskgif_type) },
};

static MP_DEFINE_CONST_DICT(gifio_module_globals, gifio_module_globals_table);

const mp_obj_module_t gifio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&gifio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_gifio, gifio_module);
