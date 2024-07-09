// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"

#include "py/mperrno.h"
#include "py/runtime.h"
#include STM32_HAL_H
#include "peripherals/periph.h"

static const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
static const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, STM32_SERIES_LOWER);
static const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, STM32_SERIES_LOWER);

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

#define RNG_TIMEOUT 5

bool common_hal_os_urandom(uint8_t *buffer, uint32_t length) {
    #if (HAS_TRNG)
    // init the RNG
    __HAL_RCC_RNG_CLK_ENABLE();
    RNG_HandleTypeDef handle;
    handle.Instance = RNG;
    if (HAL_RNG_Init(&handle) != HAL_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("RNG Init Error"));
    }

    // Assign bytes
    uint32_t i = 0;
    while (i < length) {
        uint32_t new_random;
        uint32_t start = HAL_GetTick();
        // the HAL function has a timeout, but it isn't long enough, and isn't adjustable
        while (!(__HAL_RNG_GET_FLAG(&handle, RNG_FLAG_DRDY)) && ((HAL_GetTick() - start) < RNG_TIMEOUT)) {
            ;
        }
        if (HAL_RNG_GenerateRandomNumber(&handle, &new_random) != HAL_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("Random number generation error"));
        }
        for (int j = 0; j < 4 && i < length; j++) {
            buffer[i] = new_random & 0xff;
            i++;
            new_random >>= 8;
        }
    }

    // shut down the peripheral
    if (HAL_RNG_DeInit(&handle) != HAL_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("RNG DeInit Error"));
    }
    __HAL_RCC_RNG_CLK_DISABLE();

    return true;
    #else
    return false;
    #endif
}
