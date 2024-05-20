// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/qstr.h"

#include "shared-bindings/os/__init__.h"

#ifdef SAM_D5X_E5X
#include "hal/include/hal_rand_sync.h"
#endif

static const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
#ifdef SAMD21
static const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, "samd21");
static const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, "samd21");
#endif
#ifdef SAM_D5X_E5X
static const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, "samd51");
static const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, "samd51");
#endif
static const MP_DEFINE_STR_OBJ(os_uname_info_release_obj, MICROPY_VERSION_STRING);
static const MP_DEFINE_STR_OBJ(os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE);
static const MP_DEFINE_STR_OBJ(os_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);


static MP_DEFINE_ATTRTUPLE(
    os_uname_info_obj,
    os_uname_info_fields,
    5,
    (mp_obj_t)&os_uname_info_sysname_obj,
    (mp_obj_t)&os_uname_info_nodename_obj,
    (mp_obj_t)&os_uname_info_release_obj,
    (mp_obj_t)&os_uname_info_version_obj,
    (mp_obj_t)&os_uname_info_machine_obj
    );

mp_obj_t common_hal_os_uname(void) {
    return (mp_obj_t)&os_uname_info_obj;
}

bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length) {
    #ifdef SAM_D5X_E5X
    hri_mclk_set_APBCMASK_TRNG_bit(MCLK);
    struct rand_sync_desc random;
    rand_sync_init(&random, TRNG);
    rand_sync_enable(&random);

    rand_sync_read_buf8(&random, buffer, length);

    rand_sync_disable(&random);
    rand_sync_deinit(&random);
    hri_mclk_clear_APBCMASK_TRNG_bit(MCLK);
    return true;
    #else
    return false;
    #endif
}
