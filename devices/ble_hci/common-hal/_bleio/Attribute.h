/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_ATTRIBUTE_H
#define MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_ATTRIBUTE_H

#include "shared-module/_bleio/Attribute.h"

// Types returned by attribute table lookups. These are UUIDs.
typedef enum {
  BLE_TYPE_UNKNOWN           = 0x0000,
  BLE_TYPE_PRIMARY_SERVICE   = 0x2800,
  BLE_TYPE_SECONDARY_SERVICE = 0x2801,
  BLE_TYPE_CHARACTERISTIC    = 0x2803,
  BLE_TYPE_DESCRIPTOR        = 0x2900
} ble_attribute_type;

// typedef struct
// {
//   uint8_t sm : 4;                     /**< Security Mode (1 or 2), 0 for no permissions at all. */
//   uint8_t lv : 4;                     /**< Level (1, 2, 3 or 4), 0 for no permissions at all. */

// } ble_gap_conn_sec_mode_t;

// extern void bleio_attribute_gatts_set_security_mode(ble_gap_conn_sec_mode_t *perm, bleio_attribute_security_mode_t security_mode);

#endif // MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_ATTRIBUTE_H
