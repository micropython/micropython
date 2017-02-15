/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#ifndef UBLUEPY_H__
#define UBLUEPY_H__

/* Examples:

Advertisment:

from ubluepy import Peripheral
p = Peripheral()
p.advertise(device_name="MicroPython")

DB setup:

def event_handler(id, length, data):
    print("BLE event:", id, " length: ", length)

from ubluepy import Service, Characteristic, UUID, Peripheral
u0 = UUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e")
s = Service(u0)
u1 = UUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e")
c = Characteristic(u1)
s.addCharacteristic(c)
p = Peripheral()
p.setConnectionHandler(event_handler)
p.advertise(device_name="micr", services=[s])
*/

#include "py/obj.h"

extern const mp_obj_type_t ubluepy_uuid_type;
extern const mp_obj_type_t ubluepy_service_type;

typedef enum {
    UBLUEPY_UUID_16_BIT = 1,
    UBLUEPY_UUID_128_BIT
} ubluepy_uuid_type_t;

typedef enum {
    UBLUEPY_SERVICE_PRIMARY = 1,
    UBLUEPY_SERVICE_SECONDARY = 2
} ubluepy_service_type_t;

typedef struct _ubluepy_uuid_obj_t {
    mp_obj_base_t       base;
    ubluepy_uuid_type_t type;
    uint8_t             value[2];
    uint8_t             uuid_vs_idx;
} ubluepy_uuid_obj_t;

typedef struct _ubluepy_service_obj_t {
    mp_obj_base_t        base;
    uint16_t             handle;
    uint8_t              type;
    ubluepy_uuid_obj_t * p_uuid;
} ubluepy_service_obj_t;

typedef struct _ubluepy_characteristic_obj_t {
    mp_obj_base_t        base;
    uint16_t             handle;
    ubluepy_uuid_obj_t * p_uuid;
    uint16_t             service_handle;
    uint16_t             user_desc_handle;
    uint16_t             cccd_handle;
    uint16_t             sccd_handle;
} ubluepy_characteristic_obj_t;

typedef struct _ubluepy_delegate_obj_t {
    mp_obj_base_t        base;
} ubluepy_delegate_obj_t;

typedef struct _ubluepy_peripheral_obj_t {
    mp_obj_base_t base;
    mp_obj_t      delegate;
    mp_obj_t      notif_handler;
    mp_obj_t      conn_handler;
} ubluepy_peripheral_obj_t;

typedef struct _ubluepy_advertise_data_t {
    uint8_t *  p_device_name;
    uint8_t    device_name_len;
    mp_obj_t * p_services;
    uint8_t    num_of_services;
} ubluepy_advertise_data_t;

typedef void (*ubluepy_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t length, uint8_t * data);

#endif // UBLUEPY_H__
