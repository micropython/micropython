// SPDX-FileCopyrightText: Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/gnss/SatelliteSystem.h"

//| class SatelliteSystem:
//|     """Satellite system type"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the satellite system type."""
//|
//|     GPS: SatelliteSystem
//|     """Global Positioning System."""
//|
//|     GLONASS: SatelliteSystem
//|     """GLObal NAvigation Satellite System."""
//|
//|     SBAS: SatelliteSystem
//|     """Satellite Based Augmentation System."""
//|
//|     QZSS_L1CA: SatelliteSystem
//|     """Quasi-Zenith Satellite System L1C/A."""
//|
//|     QZSS_L1S: SatelliteSystem
//|     """Quasi-Zenith Satellite System L1S."""
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
            return MP_ROM_NONE;
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
