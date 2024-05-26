// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

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
