// SPDX-FileCopyrightText: Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/gnss/GNSS.h"
#include "shared-bindings/gnss/SatelliteSystem.h"
#include "shared-bindings/gnss/PositionFix.h"

#include "shared/timeutils/timeutils.h"

extern const mp_obj_type_t gnss_type;

void common_hal_gnss_construct(gnss_obj_t *self, unsigned long selection);
void common_hal_gnss_deinit(gnss_obj_t *self);
bool common_hal_gnss_deinited(gnss_obj_t *self);
void common_hal_gnss_update(gnss_obj_t *self);

mp_float_t common_hal_gnss_get_latitude(gnss_obj_t *self);
mp_float_t common_hal_gnss_get_longitude(gnss_obj_t *self);
mp_float_t common_hal_gnss_get_altitude(gnss_obj_t *self);
void common_hal_gnss_get_timestamp(gnss_obj_t *self, timeutils_struct_time_t *tm);
gnss_positionfix_t common_hal_gnss_get_fix(gnss_obj_t *self);
