/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_BLUETOOTH_BLUETOOTH_H
#define MICROPY_INCLUDED_STM32_BLUETOOTH_BLUETOOTH_H

#include "nimble/host/include/host/ble_gap.h"

#define MP_BT_ADV_TYPE_ADV_IND          (BLE_GAP_CONN_MODE_UND)
#define MP_BT_ADV_TYPE_ADV_NONCONN_IND  (BLE_GAP_CONN_MODE_NON)

#define MP_BT_MAX_ATTR_SIZE             (20) // TODO
#define MP_BT_MAX_CONNECTED_DEVICES     (8)
#define MP_BT_INVALID_CONN_HANDLE       (0xffff)

typedef uint8_t mp_bt_adv_type_t;
typedef ble_uuid_any_t mp_bt_uuid_t;
typedef void *mp_bt_service_handle_t;
typedef uint16_t mp_bt_characteristic_handle_t;

#endif // MICROPY_INCLUDED_STM32_BLUETOOTH_BLUETOOTH_H
