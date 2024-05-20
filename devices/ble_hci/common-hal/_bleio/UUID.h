// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// Types returned by attribute table lookups. These are UUIDs.
typedef enum {
    BLE_UUID_UNKNOWN               = 0x0000,
    BLE_UUID_SERVICE_PRIMARY       = 0x2800,
    BLE_UUID_SERVICE_SECONDARY     = 0x2801,
    BLE_UUID_SERVICE_INCLUDE       = 0x2802,// not yet implemented by us
    BLE_UUID_CHARACTERISTIC        = 0x2803,
    BLE_UUID_CHAR_EXTENDED_PROPS   = 0x2900,// not yet implemented by us
    BLE_UUID_CHAR_USER_DESC        = 0x2901,// not yet implemented by us
    BLE_UUID_CCCD                  = 0x2902,
    BLE_UUID_SCCD                  = 0x2903,// not yet implemented by us
    BLE_UUID_CHAR_PRESENTATION_FMT = 0x2904, // not yet implemented by us
    BLE_UUID_CHAR_AGGREGATE_FMT    = 0x2905,// not yet implemented by us
} ble_standard_uuid;

typedef struct {
    mp_obj_base_t base;
    uint8_t size;
    uint16_t uuid16;
    uint8_t uuid128[16];
} bleio_uuid_obj_t;

uint16_t bleio_uuid_get_uuid16_or_unknown(bleio_uuid_obj_t *uuid);
