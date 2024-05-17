// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mpconfig.h"
#include "py/runtime.h"
#include "nrf_soc.h"
#include "sd_mutex.h"

void sd_mutex_acquire_check(nrf_mutex_t *p_mutex) {
    uint32_t err_code = sd_mutex_acquire(p_mutex);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("Failed to acquire mutex, err 0x%04x"), err_code);
    }
}

void sd_mutex_acquire_wait(nrf_mutex_t *p_mutex) {
    while (sd_mutex_acquire(p_mutex) == NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN) {
        RUN_BACKGROUND_TASKS;
    }
}

void sd_mutex_acquire_wait_no_vm(nrf_mutex_t *p_mutex) {
    while (sd_mutex_acquire(p_mutex) == NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN) {
    }
}

void sd_mutex_release_check(nrf_mutex_t *p_mutex) {
    uint32_t err_code = sd_mutex_release(p_mutex);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("Failed to release mutex, err 0x%04x"), err_code);
    }
}
