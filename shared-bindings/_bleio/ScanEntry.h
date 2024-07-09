// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2017 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/_bleio/ScanEntry.h"

extern const mp_obj_type_t bleio_scanentry_type;

mp_obj_t common_hal_bleio_scanentry_get_address(bleio_scanentry_obj_t *self);
mp_obj_t common_hal_bleio_scanentry_get_advertisement_bytes(bleio_scanentry_obj_t *self);
mp_int_t common_hal_bleio_scanentry_get_rssi(bleio_scanentry_obj_t *self);
bool common_hal_bleio_scanentry_get_connectable(bleio_scanentry_obj_t *self);
bool common_hal_bleio_scanentry_get_scan_response(bleio_scanentry_obj_t *self);
bool common_hal_bleio_scanentry_matches(bleio_scanentry_obj_t *self, const uint8_t *prefixes, size_t prefixes_len, bool match_all);
