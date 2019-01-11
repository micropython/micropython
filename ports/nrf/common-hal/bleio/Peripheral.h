/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_COMMON_HAL_BLEIO_PERIPHERAL_H
#define MICROPY_INCLUDED_COMMON_HAL_BLEIO_PERIPHERAL_H

#include <stdbool.h>

#include "ble.h"

#include "shared-module/bleio/__init__.h"
#include "shared-module/bleio/Address.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t name;
    gatt_role_t gatt_role;
    volatile uint16_t conn_handle;
    mp_obj_t service_list;
    mp_obj_t notif_handler;
    mp_obj_t conn_handler;
    // The advertising data buffer is held by us, not by the SD, so we must
    // maintain it and not change it. If we need to change its contents during advertising,
    // there are tricks to get the SD to notice (see DevZone - TBS).
    uint8_t adv_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];

} bleio_peripheral_obj_t;

#endif // MICROPY_INCLUDED_COMMON_HAL_BLEIO_PERIPHERAL_H
