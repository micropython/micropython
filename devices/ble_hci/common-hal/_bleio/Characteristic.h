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

#ifndef MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_CHARACTERISTIC_H
#define MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_CHARACTERISTIC_H

#include "shared-bindings/_bleio/Attribute.h"
#include "common-hal/_bleio/Descriptor.h"
#include "shared-module/_bleio/Characteristic.h"
#include "common-hal/_bleio/Service.h"
#include "common-hal/_bleio/UUID.h"

typedef struct _bleio_characteristic_obj {
    mp_obj_base_t base;
    // Will be MP_OBJ_NULL before being assigned to a Service.
    bleio_service_obj_t *service;
    bleio_uuid_obj_t *uuid;
    mp_obj_t value;
    mp_obj_t observer;
    mp_obj_list_t *descriptor_list;
    uint16_t max_length;
    bool fixed_length;
    uint16_t decl_handle;
    uint16_t handle; // Should be decl_handle+1.
    bleio_characteristic_properties_t props;
    bleio_attribute_security_mode_t read_perm;
    bleio_attribute_security_mode_t write_perm;
    bleio_descriptor_obj_t *descriptor_linked_list;
    bleio_descriptor_obj_t *user_desc;
    bleio_descriptor_obj_t *cccd;
    bleio_descriptor_obj_t *sccd;
} bleio_characteristic_obj_t;

bool bleio_characteristic_set_local_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo);

void bleio_characteristic_set_observer(bleio_characteristic_obj_t *self, mp_obj_t observer);
void bleio_characteristic_clear_observer(bleio_characteristic_obj_t *self);

#endif // MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_CHARACTERISTIC_H
