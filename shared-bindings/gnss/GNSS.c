// SPDX-FileCopyrightText: Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/gnss/GNSS.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/util.h"

#include "py/objproperty.h"
#include "py/runtime.h"

//| class GNSS:
//|     """Get updated positioning information from Global Navigation Satellite System (GNSS)
//|
//|     Usage::
//|
//|         import gnss
//|         import time
//|
//|         nav = gnss.GNSS([gnss.SatelliteSystem.GPS, gnss.SatelliteSystem.GLONASS])
//|         last_print = time.monotonic()
//|         while True:
//|             nav.update()
//|             current = time.monotonic()
//|             if current - last_print >= 1.0:
//|                 last_print = current
//|                 if nav.fix is gnss.PositionFix.INVALID:
//|                     print("Waiting for fix...")
//|                     continue
//|                 print("Latitude: {0:.6f} degrees".format(nav.latitude))
//|                 print("Longitude: {0:.6f} degrees".format(nav.longitude))"""
//|

//|     def __init__(self, system: Union[SatelliteSystem, List[SatelliteSystem]]) -> None:
//|         """Turn on the GNSS.
//|
//|         :param system: satellite system to use"""
//|         ...
STATIC mp_obj_t gnss_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    gnss_obj_t *self = mp_obj_malloc(gnss_obj_t, &gnss_type);
    enum { ARG_system };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_system, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    unsigned long selection = 0;
    if (mp_obj_is_type(args[ARG_system].u_obj, &gnss_satellitesystem_type)) {
        selection |= gnss_satellitesystem_obj_to_type(args[ARG_system].u_obj);
    } else if (mp_obj_is_type(args[ARG_system].u_obj, &mp_type_list)) {
        size_t systems_size = 0;
        mp_obj_t *systems;
        mp_obj_list_get(args[ARG_system].u_obj, &systems_size, &systems);
        for (size_t i = 0; i < systems_size; ++i) {
            if (!mp_obj_is_type(systems[i], &gnss_satellitesystem_type)) {
                mp_raise_TypeError(translate("System entry must be gnss.SatelliteSystem"));
            }
            selection |= gnss_satellitesystem_obj_to_type(systems[i]);
        }
    } else {
        mp_raise_TypeError(translate("System entry must be gnss.SatelliteSystem"));
    }

    common_hal_gnss_construct(self, selection);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Turn off the GNSS."""
//|         ...
STATIC mp_obj_t gnss_obj_deinit(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_gnss_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_deinit_obj, gnss_obj_deinit);

STATIC void check_for_deinit(gnss_obj_t *self) {
    if (common_hal_gnss_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def update(self) -> None:
//|         """Update GNSS positioning information."""
//|         ...
STATIC mp_obj_t gnss_obj_update(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_gnss_update(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_update_obj, gnss_obj_update);

//|     latitude: float
//|     """Latitude of current position in degrees (float)."""
STATIC mp_obj_t gnss_obj_get_latitude(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_gnss_get_latitude(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_get_latitude_obj, gnss_obj_get_latitude);

MP_PROPERTY_GETTER(gnss_latitude_obj,
    (mp_obj_t)&gnss_get_latitude_obj);

//|     longitude: float
//|     """Longitude of current position in degrees (float)."""
STATIC mp_obj_t gnss_obj_get_longitude(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_gnss_get_longitude(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_get_longitude_obj, gnss_obj_get_longitude);

MP_PROPERTY_GETTER(gnss_longitude_obj,
    (mp_obj_t)&gnss_get_longitude_obj);

//|     altitude: float
//|     """Altitude of current position in meters (float)."""
STATIC mp_obj_t gnss_obj_get_altitude(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_gnss_get_altitude(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_get_altitude_obj, gnss_obj_get_altitude);

MP_PROPERTY_GETTER(gnss_altitude_obj,
    (mp_obj_t)&gnss_get_altitude_obj);

//|     timestamp: time.struct_time
//|     """Time when the position data was updated."""
STATIC mp_obj_t gnss_obj_get_timestamp(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    timeutils_struct_time_t tm;
    common_hal_gnss_get_timestamp(self, &tm);
    return struct_time_from_tm(&tm);
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_get_timestamp_obj, gnss_obj_get_timestamp);

MP_PROPERTY_GETTER(gnss_timestamp_obj,
    (mp_obj_t)&gnss_get_timestamp_obj);

//|     fix: PositionFix
//|     """Fix mode."""
//|
STATIC mp_obj_t gnss_obj_get_fix(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return gnss_positionfix_type_to_obj(common_hal_gnss_get_fix(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_get_fix_obj, gnss_obj_get_fix);

MP_PROPERTY_GETTER(gnss_fix_obj,
    (mp_obj_t)&gnss_get_fix_obj);

STATIC const mp_rom_map_elem_t gnss_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&gnss_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&gnss_update_obj) },

    { MP_ROM_QSTR(MP_QSTR_latitude), MP_ROM_PTR(&gnss_latitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_longitude), MP_ROM_PTR(&gnss_longitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_altitude), MP_ROM_PTR(&gnss_altitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_timestamp), MP_ROM_PTR(&gnss_timestamp_obj) },
    { MP_ROM_QSTR(MP_QSTR_fix), MP_ROM_PTR(&gnss_fix_obj) }
};
STATIC MP_DEFINE_CONST_DICT(gnss_locals_dict, gnss_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    gnss_type,
    MP_QSTR_GNSS,
    MP_TYPE_FLAG_NONE,
    make_new, gnss_make_new,
    locals_dict, &gnss_locals_dict
    );
