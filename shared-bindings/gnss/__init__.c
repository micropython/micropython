// SPDX-FileCopyrightText: Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared-bindings/gnss/GNSS.h"
#include "shared-bindings/gnss/SatelliteSystem.h"
#include "shared-bindings/gnss/PositionFix.h"
#include "shared-bindings/util.h"

//| """Global Navigation Satellite System
//|
//| The `gnss` module contains classes to control the GNSS and acquire positioning information."""
//|
STATIC const mp_rom_map_elem_t gnss_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_gnss) },
    { MP_ROM_QSTR(MP_QSTR_GNSS), MP_ROM_PTR(&gnss_type) },

    // Enum-like Classes.
    { MP_ROM_QSTR(MP_QSTR_SatelliteSystem), MP_ROM_PTR(&gnss_satellitesystem_type) },
    { MP_ROM_QSTR(MP_QSTR_PositionFix), MP_ROM_PTR(&gnss_positionfix_type) },
};

STATIC MP_DEFINE_CONST_DICT(gnss_module_globals, gnss_module_globals_table);

const mp_obj_module_t gnss_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&gnss_module_globals,
};
