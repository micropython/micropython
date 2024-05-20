// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objtuple.h"

#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/ScanResults.h"
#include "shared-bindings/busio/UART.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#ifndef BLEIO_TOTAL_CONNECTION_COUNT
#define BLEIO_TOTAL_CONNECTION_COUNT 5
#endif

extern bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

typedef struct _bleio_adapter_obj_t {
    mp_obj_base_t base;
    bleio_scanresults_obj_t *scan_results;
    mp_obj_t name;
    mp_obj_tuple_t *connection_objs;
    busio_uart_obj_t *hci_uart;
    digitalio_digitalinout_obj_t *rts_digitalinout;
    digitalio_digitalinout_obj_t *cts_digitalinout;
    bool allocated;  // True when in use.
    bool now_advertising;
    bool extended_advertising;
    bool circuitpython_advertising;
    bool enabled;

    // HCI adapter version info.
    uint8_t hci_version;
    uint8_t lmp_version;
    uint16_t hci_revision;
    uint16_t manufacturer;
    uint16_t lmp_subversion;

    // Used to monitor advertising timeout for legacy advertising.
    uint64_t advertising_start_ticks;
    uint64_t advertising_timeout_msecs;  // If zero, do not check.

    // Generic services characteristics.
    bleio_characteristic_obj_t *device_name_characteristic;
    bleio_characteristic_obj_t *appearance_characteristic;
    bleio_characteristic_obj_t *service_changed_characteristic;

    uint16_t max_acl_buffer_len;
    uint16_t max_acl_num_buffers;
    uint16_t max_adv_data_len;
    uint8_t features[8];        // Supported BLE features.

    // All the local attributes for this device. The index into the list
    // corresponds to the handle.
    mp_obj_list_t *attributes;
    // Handle for last added service. Characteristics can only be added immediately after
    // the service they belong to. This vets that.
    uint16_t last_added_service_handle;
    uint16_t last_added_characteristic_handle;
} bleio_adapter_obj_t;

uint16_t bleio_adapter_add_attribute(bleio_adapter_obj_t *adapter, mp_obj_t *attribute);
void bleio_adapter_advertising_was_stopped(bleio_adapter_obj_t *self);
mp_obj_t *bleio_adapter_get_attribute(bleio_adapter_obj_t *adapter, uint16_t handle);
uint16_t bleio_adapter_max_attribute_handle(bleio_adapter_obj_t *adapter);
void bleio_adapter_background(bleio_adapter_obj_t *adapter);
void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter);
void bleio_adapter_reset(bleio_adapter_obj_t *adapter);
