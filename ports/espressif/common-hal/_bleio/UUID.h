// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "host/ble_uuid.h"

typedef struct {
    mp_obj_base_t base;
    // Use the native way of storing UUID's:
    // - ble_uuid_t.uuid is a 16-bit uuid.
    // - ble_uuid_t.type is BLE_UUID_TYPE_BLE if it's a 16-bit Bluetooth SIG UUID.
    //   or is BLE_UUID_TYPE_VENDOR_BEGIN and higher, which indexes into a table of registered
    //   128-bit UUIDs.
    ble_uuid_any_t nimble_ble_uuid;
} bleio_uuid_obj_t;

void bleio_uuid_construct_from_nrf_ble_uuid(bleio_uuid_obj_t *self, ble_uuid_t *nrf_uuid);
void bleio_uuid_convert_to_nrf_ble_uuid(bleio_uuid_obj_t *self, ble_uuid_t *nrf_uuid);
