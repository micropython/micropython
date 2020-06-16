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

#include "shared-bindings/gnss/GNSS.h"
#include "shared-bindings/util.h"

#include "py/objproperty.h"
#include "py/runtime.h"

//| class GNSS:
//|     """Get updated positioning information from GNSS
//|
//|     Usage::
//|
//|         import gnss
//|         import time
//|
//|         gps = gnss.GNSS()
//|         gps.select(gnss.SatelliteSystem.GPS)
//|         gps.start()
//|         last_print = time.monotonic()
//|         while True:
//|             gps.update()
//|             current = time.monotonic()
//|             if current - last_print >= 1.0:
//|                 last_print = current
//|                 if gps.fix is gnss.PositionFix.INVALID:
//|                     print("Waiting for fix...")
//|                     continue
//|                 print("Latitude: {0:.6f} degrees".format(gps.latitude))
//|                 print("Longitude: {0:.6f} degrees".format(gps.longitude))"""
//|

//|     def __init__(self, ):
//|         """Turn on the GNSS."""
//|         ...
//|
STATIC mp_obj_t gnss_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    gnss_obj_t *self = m_new_obj(gnss_obj_t);
    self->base.type = &gnss_type;

    common_hal_gnss_construct(self);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self, ) -> Any:
//|         """Turn off the GNSS."""
//|         ...
//|
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

//|     def select(self, system: gnss.SatelliteSystem) -> Any:
//|         """Add specified satellite system to selection for positioning.
//|
//|         :param gnss.SatelliteSystem system: satellite system to use"""
//|         ...
//|
STATIC mp_obj_t gnss_obj_select(mp_obj_t self_in, mp_obj_t system_obj) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    gnss_satellitesystem_t system = gnss_satellitesystem_obj_to_type(system_obj);
    common_hal_gnss_select(self, system);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(gnss_select_obj, gnss_obj_select);

//|     def deselect(self, system: gnss.SatelliteSystem) -> Any:
//|         """Remove specified satellite system from selection for positioning.
//|
//|         :param gnss.SatelliteSystem system: satellite system to remove"""
//|         ...
//|
STATIC mp_obj_t gnss_obj_deselect(mp_obj_t self_in, mp_obj_t system_obj) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    gnss_satellitesystem_t system = gnss_satellitesystem_obj_to_type(system_obj);
    common_hal_gnss_deselect(self, system);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(gnss_deselect_obj, gnss_obj_deselect);

//|     def start(self, ) -> Any:
//|         """Start positioning."""
//|         ...
//|
STATIC mp_obj_t gnss_obj_start(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_gnss_start(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_start_obj, gnss_obj_start);

//|     def stop(self, ) -> Any:
//|         """Stop positioning."""
//|         ...
//|
STATIC mp_obj_t gnss_obj_stop(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_gnss_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_stop_obj, gnss_obj_stop);

//|     def update(self, ) -> Any:
//|         """Update GNSS positioning information."""
//|         ...
//|
STATIC mp_obj_t gnss_obj_update(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_gnss_update(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_update_obj, gnss_obj_update);

//|     latitude: Any = ...
//|     """Latitude of current position."""
//|
STATIC mp_obj_t gnss_obj_get_latitude(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_gnss_get_latitude(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_get_latitude_obj, gnss_obj_get_latitude);

const mp_obj_property_t gnss_latitude_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&gnss_get_latitude_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     longitude: Any = ...
//|     """Longitude of current position."""
//|
STATIC mp_obj_t gnss_obj_get_longitude(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_gnss_get_longitude(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_get_longitude_obj, gnss_obj_get_longitude);

const mp_obj_property_t gnss_longitude_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&gnss_get_longitude_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     altitude: Any = ...
//|     """Altitude of current position."""
//|
STATIC mp_obj_t gnss_obj_get_altitude(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_gnss_get_altitude(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_get_altitude_obj, gnss_obj_get_altitude);

const mp_obj_property_t gnss_altitude_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&gnss_get_altitude_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|     fix: Any = ...
//|     """Fix mode."""
//|
STATIC mp_obj_t gnss_obj_get_fix(mp_obj_t self_in) {
    gnss_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return gnss_positionfix_type_to_obj(common_hal_gnss_get_fix(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(gnss_get_fix_obj, gnss_obj_get_fix);

const mp_obj_property_t gnss_fix_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&gnss_get_fix_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t gnss_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&gnss_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_select), MP_ROM_PTR(&gnss_select_obj) },
    { MP_ROM_QSTR(MP_QSTR_deselect), MP_ROM_PTR(&gnss_deselect_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&gnss_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&gnss_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&gnss_update_obj) },

    { MP_ROM_QSTR(MP_QSTR_latitude), MP_ROM_PTR(&gnss_latitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_longitude), MP_ROM_PTR(&gnss_longitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_altitude), MP_ROM_PTR(&gnss_altitude_obj) },
    { MP_ROM_QSTR(MP_QSTR_fix), MP_ROM_PTR(&gnss_fix_obj) }
};
STATIC MP_DEFINE_CONST_DICT(gnss_locals_dict, gnss_locals_dict_table);

const mp_obj_type_t gnss_type = {
   { &mp_type_type },
   .name = MP_QSTR_GNSS,
   .make_new = gnss_make_new,
   .locals_dict = (mp_obj_dict_t*)&gnss_locals_dict,
};
