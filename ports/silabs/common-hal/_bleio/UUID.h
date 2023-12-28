/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#ifndef MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_UUID_H
#define MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_UUID_H

#include "py/obj.h"

#define UUID16_LEN 2
#define UUID128_LEN 16

// Type of UUID
typedef enum {
    // 16-bit UUID (BT SIG assigned)
    BLE_UUID_TYPE_16 = 16,
    // 32-bit UUID (BT SIG assigned)
    BLE_UUID_TYPE_32 = 32,
    // 128-bit UUID
    BLE_UUID_TYPE_128 = 128,
} uuid_type_e;

typedef struct {
    // Type of the UUID
    uint8_t type;
} ble_uuid_t;

// 16-bit UUID
typedef struct {
    uint8_t type;
    uint16_t value;
} ble_uuid16_t;

// 32-bit UUID
typedef struct {
    uint8_t type;
    uint32_t value;
} ble_uuid32_t;

// 128-bit UUID
typedef struct {
    uint8_t type;
    uint8_t value[16];
} ble_uuid128_t;

typedef union {
    ble_uuid_t uuid;
    ble_uuid16_t uuid16;
    ble_uuid32_t uuid32;
    ble_uuid128_t uuid128;
} ble_uuid_any_t;

typedef struct {
    mp_obj_base_t base;
    // Use the native way of storing UUID's:
    // - ble_uuid_t.uuid is a 16-bit uuid.
    // - ble_uuid_t.type is BLE_UUID_TYPE_BLE if it's a 16-bit Bluetooth SIG UUID.
    //   or is BLE_UUID_TYPE_VENDOR_BEGIN and higher, which indexes into a table of registered
    //   128-bit UUIDs.
    ble_uuid_any_t efr_ble_uuid;
} bleio_uuid_obj_t;

void bleio_uuid_construct_from_efr_ble_uuid(bleio_uuid_obj_t *self,
    ble_uuid_any_t *efr_ble_uuid);
void bleio_uuid_convert_to_efr_ble_uuid(bleio_uuid_obj_t *self,
    ble_uuid_any_t *efr_ble_uuid);

#endif  // MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_UUID_H
