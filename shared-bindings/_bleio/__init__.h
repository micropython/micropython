/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO___INIT___H

#include "py/objlist.h"

#include "shared-bindings/_bleio/Adapter.h"

#include "common-hal/_bleio/__init__.h"
#include "common-hal/_bleio/Adapter.h"

extern const super_adapter_obj_t common_hal_bleio_adapter_obj;

extern void common_hal_bleio_check_connected(uint16_t conn_handle);

extern uint16_t common_hal_bleio_device_get_conn_handle(mp_obj_t device);
extern mp_obj_list_t *common_hal_bleio_device_get_remote_service_list(mp_obj_t device);
extern void common_hal_bleio_device_discover_remote_services(mp_obj_t device, mp_obj_t service_uuids_whitelist);

extern mp_obj_t common_hal_bleio_gatts_read(uint16_t handle, uint16_t conn_handle);
extern void common_hal_bleio_gatts_write(uint16_t handle, uint16_t conn_handle, mp_buffer_info_t *bufinfo);
extern void common_hal_bleio_gattc_write(uint16_t handle, uint16_t conn_handle, mp_buffer_info_t *bufinfo, bool write_no_response);


#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO___INIT___H
