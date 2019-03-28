/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
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

#pragma once

#include "esp_gap_ble_api.h"

typedef esp_ble_adv_type_t mp_bt_adv_type_t;

#define MP_BT_ADV_TYPE_ADV_IND         ADV_TYPE_IND
#define MP_BT_ADV_TYPE_ADV_NONCONN_IND ADV_TYPE_NONCONN_IND

#define MP_BT_MAX_ATTR_SIZE (20)

#define MP_BT_MAX_CONNECTED_DEVICES    CONFIG_BT_ACL_CONNECTIONS

#define MP_BT_INVALID_CONN_HANDLE      (0xffff)

void mp_bt_init(void);

typedef esp_bt_uuid_t mp_bt_uuid_t;

typedef uint16_t mp_bt_service_handle_t;

typedef uint16_t mp_bt_characteristic_handle_t;
