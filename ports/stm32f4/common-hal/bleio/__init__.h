/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#ifndef MICROPY_INCLUDED_COMMON_HAL_BLEIO_INIT_H
#define MICROPY_INCLUDED_COMMON_HAL_BLEIO_INIT_H

#include "shared-bindings/bleio/__init__.h"
#include "shared-bindings/bleio/Adapter.h"

#include "shared-module/bleio/__init__.h"

// We assume variable length data.
// 20 bytes max (23 - 3).
#define GATT_MAX_DATA_LENGTH (BLE_GATT_ATT_MTU_DEFAULT - 3)

gatt_role_t common_hal_bleio_device_get_gatt_role(mp_obj_t device);
uint16_t common_hal_bleio_device_get_conn_handle(mp_obj_t device);

#endif // MICROPY_INCLUDED_COMMON_HAL_BLEIO_INIT_H
