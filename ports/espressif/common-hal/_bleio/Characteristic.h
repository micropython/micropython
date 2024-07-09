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
#include "common-hal/_bleio/ble_events.h"
#include "common-hal/_bleio/Service.h"
#include "common-hal/_bleio/UUID.h"

#include "host/ble_gatt.h"

#define MAX_DESCRIPTORS 2

typedef struct _bleio_characteristic_obj {
    mp_obj_base_t base;
    // Will be MP_OBJ_NULL before being assigned to a Service.
    bleio_service_obj_t *service;
    bleio_uuid_obj_t *uuid;
    mp_obj_t observer;
    uint8_t *current_value;
    uint16_t current_value_len;
    // Our internal allocation length. If > 0, then current_value is managed by
    // this characteristic.
    uint16_t current_value_alloc;
    uint16_t max_length;
    uint16_t def_handle;
    uint16_t handle;
    ble_gatt_chr_flags flags;
    bleio_characteristic_properties_t props;
    bleio_attribute_security_mode_t read_perm;
    bleio_attribute_security_mode_t write_perm;
    mp_obj_list_t *descriptor_list;
    uint16_t user_desc_handle;
    uint16_t cccd_handle;
    uint16_t sccd_handle;
    ble_event_handler_entry_t event_handler_entry;
    // The actual structure is managed by the service because it needs to have
    // an array.
    struct ble_gatt_chr_def *chr_def;
    struct ble_gatt_dsc_def dsc_defs[MAX_DESCRIPTORS + 1];

    bool fixed_length;
} bleio_characteristic_obj_t;

int bleio_characteristic_access_cb(uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg);

void bleio_characteristic_set_observer(bleio_characteristic_obj_t *self, mp_obj_t observer);
void bleio_characteristic_clear_observer(bleio_characteristic_obj_t *self);
