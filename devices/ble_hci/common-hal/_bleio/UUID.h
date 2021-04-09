/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2016 Glenn Ruben Bakke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_UUID_H
#define MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_UUID_H

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

#endif // MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_UUID_H
