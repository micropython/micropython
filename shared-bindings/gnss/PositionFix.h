// SPDX-FileCopyrightText: Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_POSITIONFIX_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_POSITIONFIX_H

#include "py/obj.h"

typedef enum {
    POSITIONFIX_INVALID,
    POSITIONFIX_2D,
    POSITIONFIX_3D,
} gnss_positionfix_t;

extern const mp_obj_type_t gnss_positionfix_type;

gnss_positionfix_t gnss_positionfix_obj_to_type(mp_obj_t obj);
mp_obj_t gnss_positionfix_type_to_obj(gnss_positionfix_t mode);

typedef struct {
    mp_obj_base_t base;
} gnss_positionfix_obj_t;
extern const gnss_positionfix_obj_t gnss_positionfix_invalid_obj;
extern const gnss_positionfix_obj_t gnss_positionfix_fix2d_obj;
extern const gnss_positionfix_obj_t gnss_positionfix_fix3d_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_POSITIONFIX_H
