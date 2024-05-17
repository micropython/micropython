// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#if CIRCUITPY_DIGITALIO
#include "common-hal/digitalio/DigitalInOut.h"
int common_hal_floppyio_flux_readinto(void *buf, size_t len, digitalio_digitalinout_obj_t *data, digitalio_digitalinout_obj_t *index, mp_int_t index_wait_ms);
#endif

int common_hal_floppyio_mfm_readinto(const mp_buffer_info_t *buf, const mp_buffer_info_t *flux_buf, uint8_t *validity, size_t t2_max, size_t t3_max);
