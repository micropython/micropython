// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"

#include "shared-bindings/os/__init__.h"

#ifdef BLUETOOTH_SD
#include "nrf_sdm.h"
#endif

#include "nrf_rng.h"

static const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
static const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, "nrf52");
static const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, "nrf52");

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
    #ifdef BLUETOOTH_SD
    uint8_t sd_en = 0;
    (void)sd_softdevice_is_enabled(&sd_en);

    if (sd_en) {
        while (length != 0) {
            uint8_t available = 0;
            sd_rand_application_bytes_available_get(&available);
            if (available) {
                uint32_t request = MIN(length, available);
                uint32_t result = sd_rand_application_vector_get(buffer, request);
                if (result != NRF_SUCCESS) {
                    return false;
                }
                buffer += request;
                length -= request;
            } else {
                RUN_BACKGROUND_TASKS;
            }
        }
        return true;
    }
    #endif

    nrf_rng_event_clear(NRF_RNG, NRF_RNG_EVENT_VALRDY);
    nrf_rng_task_trigger(NRF_RNG, NRF_RNG_TASK_START);

    for (uint32_t i = 0; i < length; i++) {
        while (nrf_rng_event_check(NRF_RNG, NRF_RNG_EVENT_VALRDY) == 0) {
            ;
        }
        nrf_rng_event_clear(NRF_RNG, NRF_RNG_EVENT_VALRDY);

        buffer[i] = nrf_rng_random_value_get(NRF_RNG);
    }

    nrf_rng_task_trigger(NRF_RNG, NRF_RNG_TASK_STOP);

    return true;
}
