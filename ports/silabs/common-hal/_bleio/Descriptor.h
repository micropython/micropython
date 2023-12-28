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

#ifndef MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_DESCRIPTOR_H
#define MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_DESCRIPTOR_H

#include "py/obj.h"
#include "common-hal/_bleio/UUID.h"
#include "shared-module/_bleio/Attribute.h"

#define BLE_ATT_ATTR_MAX_LEN 50

// Forward declare characteristic because it includes a Descriptor.
struct _bleio_characteristic_obj;

typedef struct _bleio_descriptor_obj {
    mp_obj_base_t base;
    // Will be MP_OBJ_NULL before being assigned to a Characteristic.
    struct _bleio_characteristic_obj *characteristic;
    bleio_uuid_obj_t *uuid;
    mp_obj_t initial_value;
    uint16_t max_length;
    bool fixed_length;
    uint16_t handle;
    // struct ble_gatt_dsc_def def;
    bleio_attribute_security_mode_t read_perm;
    bleio_attribute_security_mode_t write_perm;
} bleio_descriptor_obj_t;

#endif // MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_DESCRIPTOR_H
