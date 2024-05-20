// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/qstr.h"

#include "shared-bindings/os/__init__.h"

#if CIRCUITPY_RANDOM
#include "sdk/drivers/trng/fsl_trng.h"
#endif

static const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
static const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, "mimxrt10xx");
static const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, "mimxrt10xx");
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
    #if CIRCUITPY_RANDOM
    trng_config_t trngConfig;

    TRNG_GetDefaultConfig(&trngConfig);
    trngConfig.sampleMode = kTRNG_SampleModeVonNeumann;

    TRNG_Init(TRNG, &trngConfig);
    TRNG_GetRandomData(TRNG, buffer, length);
    TRNG_Deinit(TRNG);

    return true;
    #else
    return false;
    #endif
}
