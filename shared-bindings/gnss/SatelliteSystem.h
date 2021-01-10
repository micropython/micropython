// SPDX-FileCopyrightText: Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_SATELLITESYSTEM_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_SATELLITESYSTEM_H

#include "py/obj.h"

typedef enum {
    SATELLITESYSTEM_NONE       = 0,
    SATELLITESYSTEM_GPS        = (1U << 0),
    SATELLITESYSTEM_GLONASS    = (1U << 1),
    SATELLITESYSTEM_SBAS       = (1U << 2),
    SATELLITESYSTEM_QZSS_L1CA  = (1U << 3),
    SATELLITESYSTEM_QZSS_L1S   = (1U << 4),
} gnss_satellitesystem_t;

extern const mp_obj_type_t gnss_satellitesystem_type;

gnss_satellitesystem_t gnss_satellitesystem_obj_to_type(mp_obj_t obj);
mp_obj_t gnss_satellitesystem_type_to_obj(gnss_satellitesystem_t mode);

typedef struct {
    mp_obj_base_t base;
} gnss_satellitesystem_obj_t;
extern const gnss_satellitesystem_obj_t gnss_satellitesystem_gps_obj;
extern const gnss_satellitesystem_obj_t gnss_satellitesystem_glonass_obj;
extern const gnss_satellitesystem_obj_t gnss_satellitesystem_sbas_obj;
extern const gnss_satellitesystem_obj_t gnss_satellitesystem_qzss_l1ca_obj;
extern const gnss_satellitesystem_obj_t gnss_satellitesystem_qzss_l1s_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_GNSS_SATELLITESYSTEM_H
