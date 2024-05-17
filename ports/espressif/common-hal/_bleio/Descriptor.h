// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/_bleio/UUID.h"

#include "host/ble_gatt.h"

// Forward declare characteristic because it includes a Descriptor.
struct _bleio_characteristic_obj;

typedef struct _bleio_descriptor_obj {
    mp_obj_base_t base;
    // Will be MP_OBJ_NULL before being assigned to a Characteristic.
    struct _bleio_characteristic_obj *characteristic;
    bleio_uuid_obj_t *uuid;
    mp_obj_t initial_value;
    uint16_t max_length;
    bool fixed_length;
    uint16_t handle;
    struct ble_gatt_dsc_def *dsc_def;
    uint8_t flags;
    bleio_attribute_security_mode_t read_perm;
    bleio_attribute_security_mode_t write_perm;
} bleio_descriptor_obj_t;

int bleio_descriptor_access_cb(uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg);
