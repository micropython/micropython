// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/audiotap/Tap.h"

//| """Look at audio as it plays
//|
//| The `audiotap` module contains `Tap`, which goes into an audio chain like
//| the effects in `audiofilters` and `audiodelays`. Audio passes through it
//| unchanged, and the program can read a copy of it and its levels, for meters,
//| waveforms, spectrum analysis or recording, while it plays."""

static const mp_rom_map_elem_t audiotap_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiotap) },
    { MP_ROM_QSTR(MP_QSTR_Tap), MP_ROM_PTR(&audiotap_tap_type) },
};

static MP_DEFINE_CONST_DICT(audiotap_module_globals, audiotap_module_globals_table);

const mp_obj_module_t audiotap_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiotap_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audiotap, audiotap_module);
