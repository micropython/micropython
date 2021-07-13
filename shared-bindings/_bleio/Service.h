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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_SERVICE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_SERVICE_H

#include "common-hal/_bleio/Characteristic.h"
#include "common-hal/_bleio/Connection.h"
#include "common-hal/_bleio/Service.h"

#include "py/objtuple.h"

extern const mp_obj_type_t bleio_service_type;

// Private version that doesn't allocate on the heap
extern uint32_t _common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary, mp_obj_list_t *characteristic_list);
extern void common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary);
extern void common_hal_bleio_service_from_remote_service(bleio_service_obj_t *self, bleio_connection_obj_t *connection, bleio_uuid_obj_t *uuid, bool is_secondary);
extern bleio_uuid_obj_t *common_hal_bleio_service_get_uuid(bleio_service_obj_t *self);
extern mp_obj_tuple_t *common_hal_bleio_service_get_characteristics(bleio_service_obj_t *self);
extern bool common_hal_bleio_service_get_is_remote(bleio_service_obj_t *self);
extern bool common_hal_bleio_service_get_is_secondary(bleio_service_obj_t *self);
extern void common_hal_bleio_service_add_characteristic(bleio_service_obj_t *self, bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *initial_value_bufinfo, const char *user_description);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_SERVICE_H
