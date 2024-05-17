// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_ATTRIBUTE_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_ATTRIBUTE_H

#include "shared-module/_bleio/Attribute.h"

extern void bleio_attribute_gatts_set_security_mode(ble_gap_conn_sec_mode_t *perm, bleio_attribute_security_mode_t security_mode);

#endif // MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_ATTRIBUTE_H
