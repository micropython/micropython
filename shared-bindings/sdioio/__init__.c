// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT


#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/sdioio/SDCard.h"

//| """Interface to an SD card via the SDIO bus"""

static const mp_rom_map_elem_t sdioio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sdio) },
    { MP_ROM_QSTR(MP_QSTR_SDCard), MP_ROM_PTR(&sdioio_SDCard_type) },
};

static MP_DEFINE_CONST_DICT(sdioio_module_globals, sdioio_module_globals_table);

const mp_obj_module_t sdioio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sdioio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_sdioio, sdioio_module);
