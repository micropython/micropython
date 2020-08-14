/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_ADAPTER_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_ADAPTER_H

#include <stdint.h>

#include "common-hal/_bleio/Adapter.h"

#include "py/objstr.h"
#include "shared-module/_bleio/Address.h"

const mp_obj_type_t bleio_adapter_type;

extern bool common_hal_bleio_adapter_get_advertising(bleio_adapter_obj_t *self);
extern bool common_hal_bleio_adapter_get_enabled(bleio_adapter_obj_t *self);
extern void common_hal_bleio_adapter_set_enabled(bleio_adapter_obj_t *self, bool enabled);
extern bool common_hal_bleio_adapter_get_connected(bleio_adapter_obj_t *self);
extern bleio_address_obj_t *common_hal_bleio_adapter_get_address(bleio_adapter_obj_t *self);
extern bool common_hal_bleio_adapter_set_address(bleio_adapter_obj_t *self, bleio_address_obj_t *address);

extern mp_obj_str_t* common_hal_bleio_adapter_get_name(bleio_adapter_obj_t *self);
extern void common_hal_bleio_adapter_set_name(bleio_adapter_obj_t *self, const char* name);

extern uint32_t _common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self, bool connectable, bool anonymous, uint32_t timeout, float interval, uint8_t *advertising_data, uint16_t advertising_data_len, uint8_t *scan_response_data, uint16_t scan_response_data_len);

extern void common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self, bool connectable, bool anonymous, uint32_t timeout, mp_float_t interval, mp_buffer_info_t *advertising_data_bufinfo, mp_buffer_info_t *scan_response_data_bufinfo);
extern void common_hal_bleio_adapter_stop_advertising(bleio_adapter_obj_t *self);

extern mp_obj_t common_hal_bleio_adapter_start_scan(bleio_adapter_obj_t *self, uint8_t* prefixes, size_t prefix_length, bool extended, mp_int_t buffer_size, mp_float_t timeout, mp_float_t interval, mp_float_t window, mp_int_t minimum_rssi, bool active);
extern void common_hal_bleio_adapter_stop_scan(bleio_adapter_obj_t *self);

extern bool common_hal_bleio_adapter_get_connected(bleio_adapter_obj_t *self);
extern mp_obj_t common_hal_bleio_adapter_get_connections(bleio_adapter_obj_t *self);
extern mp_obj_t common_hal_bleio_adapter_connect(bleio_adapter_obj_t *self, bleio_address_obj_t *address, mp_float_t timeout);

extern void common_hal_bleio_adapter_erase_bonding(bleio_adapter_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_ADAPTER_H
