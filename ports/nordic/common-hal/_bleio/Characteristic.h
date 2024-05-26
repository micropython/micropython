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
    const uint8_t *initial_value;
    uint16_t initial_value_len;
    uint16_t max_length;
    uint16_t handle;
    bleio_characteristic_properties_t props;
    bleio_attribute_security_mode_t read_perm;
    bleio_attribute_security_mode_t write_perm;
    mp_obj_list_t *descriptor_list;
    uint16_t user_desc_handle;
    uint16_t cccd_handle;
    uint16_t sccd_handle;
    bool fixed_length;
} bleio_characteristic_obj_t;
