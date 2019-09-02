/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_PERIPHERAL_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_PERIPHERAL_H

#include <stdbool.h>

#include "ble.h"

#include "py/obj.h"
#include "py/objlist.h"

#include "common-hal/_bleio/__init__.h"
#include "shared-module/_bleio/Address.h"

typedef enum {
    PAIR_NOT_PAIRED,
    PAIR_WAITING,
    PAIR_PAIRED,
} pair_status_t;

typedef struct {
    mp_obj_base_t base;
    mp_obj_t name;
    volatile uint16_t conn_handle;
    // Services provided by this peripheral.
    mp_obj_list_t *service_list;
    // Remote services discovered when this peripheral is acting as a client.
    mp_obj_list_t *remote_service_list;
    // The advertising data and scan response buffers are held by us, not by the SD, so we must
    // maintain them and not change it. If we need to change the contents during advertising,
    // there are tricks to get the SD to notice (see DevZone - TBS).
    // EDIV: Encrypted Diversifier: Identifies LTK during legacy pairing.
    bonding_keys_t bonding_keys;
    uint16_t ediv;
    uint8_t* advertising_data;
    uint8_t* scan_response_data;
    uint8_t adv_handle;
    pair_status_t pair_status;
} bleio_peripheral_obj_t;

#endif // MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_PERIPHERAL_H
