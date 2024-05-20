// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/imagecapture/ParallelImageCapture.h"

//| """Support for "Parallel capture" interfaces
//|
//| .. seealso::
//|
//|     Espressif microcontrollers use the `espcamera` module together.
//|
//| """
static const mp_rom_map_elem_t imagecapture_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_imagecapture) },
    { MP_ROM_QSTR(MP_QSTR_ParallelImageCapture), MP_ROM_PTR(&imagecapture_parallelimagecapture_type) },
};

static MP_DEFINE_CONST_DICT(imagecapture_module_globals, imagecapture_module_globals_table);

const mp_obj_module_t imagecapture_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&imagecapture_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_imagecapture, imagecapture_module);
