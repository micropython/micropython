// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT


#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/sdcardio/SDCard.h"

//| """Interface to an SD card via the SPI bus"""

static const mp_rom_map_elem_t sdcardio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sdcardio) },
    { MP_ROM_QSTR(MP_QSTR_SDCard), MP_ROM_PTR(&sdcardio_SDCard_type) },
};

static MP_DEFINE_CONST_DICT(sdcardio_module_globals, sdcardio_module_globals_table);

const mp_obj_module_t sdcardio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sdcardio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_sdcardio, sdcardio_module);
