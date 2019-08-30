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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_PERIPHERAL_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_PERIPHERAL_H

#include "py/objtuple.h"
#include "common-hal/_bleio/Peripheral.h"
#include "common-hal/_bleio/Service.h"

extern const mp_obj_type_t bleio_peripheral_type;

extern void common_hal_bleio_peripheral_construct(bleio_peripheral_obj_t *self, mp_obj_t name);
extern void common_hal_bleio_peripheral_add_service(bleio_peripheral_obj_t *self, bleio_service_obj_t *service);
extern mp_obj_list_t *common_hal_bleio_peripheral_get_services(bleio_peripheral_obj_t *self);
extern bool common_hal_bleio_peripheral_get_connected(bleio_peripheral_obj_t *self);
extern mp_obj_t common_hal_bleio_peripheral_get_name(bleio_peripheral_obj_t *self);
extern void common_hal_bleio_peripheral_start_advertising(bleio_peripheral_obj_t *device, bool connectable, float interval, mp_buffer_info_t *advertising_data_bufinfo, mp_buffer_info_t *scan_response_data_bufinfo);
extern void common_hal_bleio_peripheral_stop_advertising(bleio_peripheral_obj_t *device);
extern void common_hal_bleio_peripheral_disconnect(bleio_peripheral_obj_t *device);
extern mp_obj_tuple_t *common_hal_bleio_peripheral_discover_remote_services(bleio_peripheral_obj_t *self, mp_obj_t service_uuids_whitelist);
extern void common_hal_bleio_peripheral_pair(bleio_peripheral_obj_t *device);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_PERIPHERAL_H
