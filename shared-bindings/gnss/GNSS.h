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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_GNSS_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_GNSS_H

#include "common-hal/gnss/GNSS.h"
#include "shared-bindings/gnss/SatelliteSystem.h"
#include "shared-bindings/gnss/PositionFix.h"

extern const mp_obj_type_t gnss_type;

void common_hal_gnss_construct(gnss_obj_t *self);
void common_hal_gnss_deinit(gnss_obj_t *self);
bool common_hal_gnss_deinited(gnss_obj_t *self);
void common_hal_gnss_select(gnss_obj_t *self, gnss_satellitesystem_t system);
void common_hal_gnss_deselect(gnss_obj_t *self, gnss_satellitesystem_t system);
void common_hal_gnss_start(gnss_obj_t *self);
void common_hal_gnss_stop(gnss_obj_t *self);
void common_hal_gnss_update(gnss_obj_t *self);

mp_float_t common_hal_gnss_get_latitude(gnss_obj_t *self);
mp_float_t common_hal_gnss_get_longitude(gnss_obj_t *self);
mp_float_t common_hal_gnss_get_altitude(gnss_obj_t *self);
gnss_positionfix_t common_hal_gnss_get_fix(gnss_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_GNSS_H
