// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objtuple.h"
#include "shared-bindings/_bleio/Attribute.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-module/_bleio/Characteristic.h"
#include "common-hal/_bleio/Characteristic.h"
#include "common-hal/_bleio/Service.h"

extern const mp_obj_type_t bleio_characteristic_type;

extern bleio_characteristic_properties_t common_hal_bleio_characteristic_get_properties(bleio_characteristic_obj_t *self);
extern mp_obj_tuple_t *common_hal_bleio_characteristic_get_descriptors(bleio_characteristic_obj_t *self);
extern bleio_service_obj_t *common_hal_bleio_characteristic_get_service(bleio_characteristic_obj_t *self);
extern bleio_uuid_obj_t *common_hal_bleio_characteristic_get_uuid(bleio_characteristic_obj_t *self);
extern size_t common_hal_bleio_characteristic_get_max_length(bleio_characteristic_obj_t *self);
extern size_t common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self, uint8_t *buf, size_t len);
extern void common_hal_bleio_characteristic_add_descriptor(bleio_characteristic_obj_t *self, bleio_descriptor_obj_t *descriptor);
extern void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self, bleio_service_obj_t *service, uint16_t handle, bleio_uuid_obj_t *uuid, bleio_characteristic_properties_t props, bleio_attribute_security_mode_t read_perm, bleio_attribute_security_mode_t write_perm, mp_int_t max_length, bool fixed_length, mp_buffer_info_t *initial_value_bufinfo, const char *user_description);
extern void common_hal_bleio_characteristic_set_cccd(bleio_characteristic_obj_t *self, bool notify, bool indicate);
extern void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo);
