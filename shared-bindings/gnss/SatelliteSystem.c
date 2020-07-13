/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2020 Sony Semiconductor Solutions Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/gnss/SatelliteSystem.h"

//| class SatelliteSystem:
//|     """Satellite system type"""
//|
//|     def __init__(self, ):
//|         """Enum-like class to define the satellite system type."""
//|
//|     GPS: Any = ...
//|     """Global Positioning System.
//|
//|     :type gnss.SatelliteSystem:"""
//|
//|     GLONASS: Any = ...
//|     """GLObal NAvigation Satellite System.
//|
//|     :type gnss.SatelliteSystem:"""
//|
//|     SBAS: Any = ...
//|     """Satellite Based Augmentation System.
//|
//|     :type gnss.SatelliteSystem:"""
//|
//|     QZSS_L1CA: Any = ...
//|     """Quasi-Zenith Satellite System L1C/A.
//|
//|     :type gnss.SatelliteSystem:"""
//|
//|     QZSS_L1S: Any = ...
//|     """Quasi-Zenith Satellite System L1S.
//|
//|     :type gnss.SatelliteSystem:"""
//|
const mp_obj_type_t gnss_satellitesystem_type;

const gnss_satellitesystem_obj_t gnss_satellitesystem_gps_obj = {
    { &gnss_satellitesystem_type },
};

const gnss_satellitesystem_obj_t gnss_satellitesystem_glonass_obj = {
    { &gnss_satellitesystem_type },
};

const gnss_satellitesystem_obj_t gnss_satellitesystem_sbas_obj = {
    { &gnss_satellitesystem_type },
};

const gnss_satellitesystem_obj_t gnss_satellitesystem_qzss_l1ca_obj = {
    { &gnss_satellitesystem_type },
};

const gnss_satellitesystem_obj_t gnss_satellitesystem_qzss_l1s_obj = {
    { &gnss_satellitesystem_type },
};

gnss_satellitesystem_t gnss_satellitesystem_obj_to_type(mp_obj_t obj) {
    if (obj == MP_ROM_PTR(&gnss_satellitesystem_gps_obj)) {
        return SATELLITESYSTEM_GPS;
    } else if (obj == MP_ROM_PTR(&gnss_satellitesystem_glonass_obj)) {
        return SATELLITESYSTEM_GLONASS;
    } else if (obj == MP_ROM_PTR(&gnss_satellitesystem_sbas_obj)) {
        return SATELLITESYSTEM_SBAS;
    } else if (obj == MP_ROM_PTR(&gnss_satellitesystem_qzss_l1ca_obj)) {
        return SATELLITESYSTEM_QZSS_L1CA;
    } else if (obj == MP_ROM_PTR(&gnss_satellitesystem_qzss_l1s_obj)) {
        return SATELLITESYSTEM_QZSS_L1S;
    }
    return SATELLITESYSTEM_NONE;
}

mp_obj_t gnss_satellitesystem_type_to_obj(gnss_satellitesystem_t system) {
    switch (system) {
        case SATELLITESYSTEM_GPS:
            return (mp_obj_t)MP_ROM_PTR(&gnss_satellitesystem_gps_obj);
        case SATELLITESYSTEM_GLONASS:
            return (mp_obj_t)MP_ROM_PTR(&gnss_satellitesystem_glonass_obj);
        case SATELLITESYSTEM_SBAS:
            return (mp_obj_t)MP_ROM_PTR(&gnss_satellitesystem_sbas_obj);
        case SATELLITESYSTEM_QZSS_L1CA:
            return (mp_obj_t)MP_ROM_PTR(&gnss_satellitesystem_qzss_l1ca_obj);
        case SATELLITESYSTEM_QZSS_L1S:
            return (mp_obj_t)MP_ROM_PTR(&gnss_satellitesystem_qzss_l1s_obj);
        case SATELLITESYSTEM_NONE:
        default:
            return (mp_obj_t)MP_ROM_PTR(&mp_const_none_obj);
    }
}

STATIC const mp_rom_map_elem_t gnss_satellitesystem_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_GPS), MP_ROM_PTR(&gnss_satellitesystem_gps_obj)},
    {MP_ROM_QSTR(MP_QSTR_GLONASS), MP_ROM_PTR(&gnss_satellitesystem_glonass_obj)},
    {MP_ROM_QSTR(MP_QSTR_SBAS), MP_ROM_PTR(&gnss_satellitesystem_sbas_obj)},
    {MP_ROM_QSTR(MP_QSTR_QZSS_L1CA), MP_ROM_PTR(&gnss_satellitesystem_qzss_l1ca_obj)},
    {MP_ROM_QSTR(MP_QSTR_QZSS_L1S), MP_ROM_PTR(&gnss_satellitesystem_qzss_l1s_obj)},
};
STATIC MP_DEFINE_CONST_DICT(gnss_satellitesystem_locals_dict, gnss_satellitesystem_locals_dict_table);

STATIC void gnss_satellitesystem_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr system = MP_QSTR_None;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&gnss_satellitesystem_gps_obj)) {
        system = MP_QSTR_GPS;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&gnss_satellitesystem_glonass_obj)) {
        system = MP_QSTR_GLONASS;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&gnss_satellitesystem_sbas_obj)) {
        system = MP_QSTR_SBAS;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&gnss_satellitesystem_qzss_l1ca_obj)) {
        system = MP_QSTR_QZSS_L1CA;
    } else if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&gnss_satellitesystem_qzss_l1s_obj)) {
        system = MP_QSTR_QZSS_L1S;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_gnss, MP_QSTR_SatelliteSystem, system);
}

const mp_obj_type_t gnss_satellitesystem_type = {
    { &mp_type_type },
    .name = MP_QSTR_SatelliteSystem,
    .print = gnss_satellitesystem_print,
    .locals_dict = (mp_obj_t)&gnss_satellitesystem_locals_dict,
};
