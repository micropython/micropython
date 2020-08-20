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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_CONNECTION_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_CONNECTION_H

#include "py/objtuple.h"
#include "common-hal/_bleio/Connection.h"
#include "common-hal/_bleio/Service.h"

extern const mp_obj_type_t bleio_connection_type;

void common_hal_bleio_connection_pair(bleio_connection_internal_t *self, bool bond);
void common_hal_bleio_connection_disconnect(bleio_connection_internal_t *self);
bool common_hal_bleio_connection_get_connected(bleio_connection_obj_t *self);
mp_int_t common_hal_bleio_connection_get_max_packet_length(bleio_connection_internal_t *self);
bool common_hal_bleio_connection_get_paired(bleio_connection_obj_t *self);
mp_obj_tuple_t *common_hal_bleio_connection_discover_remote_services(bleio_connection_obj_t *self, mp_obj_t service_uuids_whitelist);

mp_float_t common_hal_bleio_connection_get_connection_interval(bleio_connection_internal_t *self);
void common_hal_bleio_connection_set_connection_interval(bleio_connection_internal_t *self, mp_float_t new_interval);

void bleio_connection_ensure_connected(bleio_connection_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_CONNECTION_H
