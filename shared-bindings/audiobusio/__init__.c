// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audiobusio/__init__.h"
#include "shared-bindings/audiobusio/I2SOut.h"
#include "shared-bindings/audiobusio/PDMIn.h"

//| """Support for audio input and output over digital buses
//|
//| The `audiobusio` module contains classes to provide access to audio IO
//| over digital buses. These protocols are used to communicate audio to other
//| chips in the same circuit. It doesn't include audio interconnect protocols
//| such as S/PDIF.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed. To do so, either call :py:meth:`!deinit` or use a
//| context manager."""

static const mp_rom_map_elem_t audiobusio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiobusio) },
    { MP_ROM_QSTR(MP_QSTR_I2SOut), MP_ROM_PTR(&audiobusio_i2sout_type) },
    { MP_ROM_QSTR(MP_QSTR_PDMIn), MP_ROM_PTR(&audiobusio_pdmin_type) },
};

static MP_DEFINE_CONST_DICT(audiobusio_module_globals, audiobusio_module_globals_table);

const mp_obj_module_t audiobusio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiobusio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audiobusio, audiobusio_module);
