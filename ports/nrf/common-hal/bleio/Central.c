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

#include <string.h>
#include <stdio.h>

#include "ble.h"
#include "ble_drv.h"
#include "ble_hci.h"
#include "nrf_soc.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/bleio/Adapter.h"
#include "shared-bindings/bleio/Characteristic.h"
#include "shared-bindings/bleio/Central.h"
#include "shared-bindings/bleio/Descriptor.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"

static bleio_service_obj_t *m_char_discovery_service;
static bleio_characteristic_obj_t *m_desc_discovery_characteristic;

static volatile bool m_discovery_in_process;
static volatile bool m_discovery_successful;

// service_uuid may be NULL, to discover all services.
STATIC bool discover_next_services(bleio_central_obj_t *self, uint16_t start_handle, ble_uuid_t *service_uuid) {
    m_discovery_successful = false;
    m_discovery_in_process = true;

    uint32_t err_code = sd_ble_gattc_primary_services_discover(self->conn_handle, start_handle, service_uuid);

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to discover services"));
    }

    // Wait for a discovery event.
    while (m_discovery_in_process) {
        MICROPY_VM_HOOK_LOOP;
    }
    return m_discovery_successful;
}

STATIC bool discover_next_characteristics(bleio_central_obj_t *self, bleio_service_obj_t *service, uint16_t start_handle) {
    m_char_discovery_service = service;

    ble_gattc_handle_range_t handle_range;
    handle_range.start_handle = start_handle;
    handle_range.end_handle = service->end_handle;

    m_discovery_successful = false;
    m_discovery_in_process = true;

    uint32_t err_code = sd_ble_gattc_characteristics_discover(self->conn_handle, &handle_range);
    if (err_code != NRF_SUCCESS) {
        return false;
    }

    // Wait for a discovery event.
    while (m_discovery_in_process) {
        MICROPY_VM_HOOK_LOOP;
    }
    return m_discovery_successful;
}

STATIC bool discover_next_descriptors(bleio_central_obj_t *self, bleio_characteristic_obj_t *characteristic, uint16_t start_handle, uint16_t end_handle) {
    m_desc_discovery_characteristic = characteristic;

    ble_gattc_handle_range_t handle_range;
    handle_range.start_handle = start_handle;
    handle_range.end_handle = end_handle;

    m_discovery_successful = false;
    m_discovery_in_process = true;

    uint32_t err_code = sd_ble_gattc_descriptors_discover(self->conn_handle, &handle_range);
    if (err_code != NRF_SUCCESS) {
        return false;
    }

    // Wait for a discovery event.
    while (m_discovery_in_process) {
        MICROPY_VM_HOOK_LOOP;
    }
    return m_discovery_successful;
}

STATIC void on_primary_srv_discovery_rsp(ble_gattc_evt_prim_srvc_disc_rsp_t *response, bleio_central_obj_t *central) {
    for (size_t i = 0; i < response->count; ++i) {
        ble_gattc_service_t *gattc_service = &response->services[i];

        bleio_service_obj_t *service = m_new_obj(bleio_service_obj_t);
        service->base.type = &bleio_service_type;

        // Initialize several fields at once.
        common_hal_bleio_service_construct(service, NULL, mp_obj_new_list(0, NULL), false);

        service->device = MP_OBJ_FROM_PTR(central);
        service->start_handle = gattc_service->handle_range.start_handle;
        service->end_handle = gattc_service->handle_range.end_handle;
        service->handle = gattc_service->handle_range.start_handle;

        if (gattc_service->uuid.type != BLE_UUID_TYPE_UNKNOWN) {
            // Known service UUID.
            bleio_uuid_obj_t *uuid = m_new_obj(bleio_uuid_obj_t);
            uuid->base.type = &bleio_uuid_type;
            bleio_uuid_construct_from_nrf_ble_uuid(uuid, &gattc_service->uuid);
            service->uuid = uuid;
            service->device = MP_OBJ_FROM_PTR(central);
        } else {
            // The discovery response contained a 128-bit UUID that has not yet been registered with the
            // softdevice via sd_ble_uuid_vs_add(). We need to fetch the 128-bit value and register it.
            // For now, just set the UUID to NULL.
            service->uuid = NULL;
        }

        mp_obj_list_append(central->service_list, service);
    }

    if (response->count > 0) {
        m_discovery_successful = true;
    }
    m_discovery_in_process = false;
}

STATIC void on_char_discovery_rsp(ble_gattc_evt_char_disc_rsp_t *response, bleio_central_obj_t *central) {
    for (size_t i = 0; i < response->count; ++i) {
        ble_gattc_char_t *gattc_char = &response->chars[i];

        bleio_characteristic_obj_t *characteristic = m_new_obj(bleio_characteristic_obj_t);
        characteristic->base.type = &bleio_characteristic_type;

        characteristic->descriptor_list = mp_obj_new_list(0, NULL);

        bleio_uuid_obj_t *uuid = NULL;

        if (gattc_char->uuid.type != BLE_UUID_TYPE_UNKNOWN) {
            // Known characteristic UUID.
            uuid = m_new_obj(bleio_uuid_obj_t);
            uuid->base.type = &bleio_uuid_type;
            bleio_uuid_construct_from_nrf_ble_uuid(uuid, &gattc_char->uuid);
        } else {
            // The discovery response contained a 128-bit UUID that has not yet been registered with the
            // softdevice via sd_ble_uuid_vs_add(). We need to fetch the 128-bit value and register it.
            // For now, just leave the UUID as NULL.
        }

        bleio_characteristic_properties_t props;

        props.broadcast = gattc_char->char_props.broadcast;
        props.indicate = gattc_char->char_props.indicate;
        props.notify = gattc_char->char_props.notify;
        props.read = gattc_char->char_props.read;
        props.write = gattc_char->char_props.write;
        props.write_no_response = gattc_char->char_props.write_wo_resp;

        // Call common_hal_bleio_characteristic_construct() to initalize some fields and set up evt handler.
        common_hal_bleio_characteristic_construct(characteristic, uuid, props, mp_obj_new_list(0, NULL));
        characteristic->handle = gattc_char->handle_value;
        characteristic->service = m_char_discovery_service;

        mp_obj_list_append(m_char_discovery_service->characteristic_list, MP_OBJ_FROM_PTR(characteristic));
    }

    if (response->count > 0) {
        m_discovery_successful = true;
    }
    m_discovery_in_process = false;
}

STATIC void on_desc_discovery_rsp(ble_gattc_evt_desc_disc_rsp_t *response, bleio_central_obj_t *central) {
    for (size_t i = 0; i < response->count; ++i) {
        ble_gattc_desc_t *gattc_desc = &response->descs[i];

        // Remember handles for certain well-known descriptors.
        switch (gattc_desc->uuid.uuid) {
            case DESCRIPTOR_UUID_CLIENT_CHARACTERISTIC_CONFIGURATION:
                m_desc_discovery_characteristic->cccd_handle = gattc_desc->handle;
                break;

            case DESCRIPTOR_UUID_SERVER_CHARACTERISTIC_CONFIGURATION:
                m_desc_discovery_characteristic->sccd_handle = gattc_desc->handle;
                break;

            case DESCRIPTOR_UUID_CHARACTERISTIC_USER_DESCRIPTION:
                m_desc_discovery_characteristic->user_desc_handle = gattc_desc->handle;
                break;

            default:
                // TODO: sd_ble_gattc_descriptors_discover() can return things that are not descriptors,
                // so ignore those.
                // https://devzone.nordicsemi.com/f/nordic-q-a/49500/sd_ble_gattc_descriptors_discover-is-returning-attributes-that-are-not-descriptors
                break;
        }

        bleio_descriptor_obj_t *descriptor = m_new_obj(bleio_descriptor_obj_t);
        descriptor->base.type = &bleio_descriptor_type;

        bleio_uuid_obj_t *uuid = NULL;

        if (gattc_desc->uuid.type != BLE_UUID_TYPE_UNKNOWN) {
            // Known descriptor UUID.
            uuid = m_new_obj(bleio_uuid_obj_t);
            uuid->base.type = &bleio_uuid_type;
            bleio_uuid_construct_from_nrf_ble_uuid(uuid, &gattc_desc->uuid);
        } else {
            // The discovery response contained a 128-bit UUID that has not yet been registered with the
            // softdevice via sd_ble_uuid_vs_add(). We need to fetch the 128-bit value and register it.
            // For now, just leave the UUID as NULL.
        }

        common_hal_bleio_descriptor_construct(descriptor, uuid);
        descriptor->handle = gattc_desc->handle;
        descriptor->characteristic = m_desc_discovery_characteristic;

        mp_obj_list_append(m_desc_discovery_characteristic->descriptor_list, MP_OBJ_FROM_PTR(descriptor));
    }

    if (response->count > 0) {
        m_discovery_successful = true;
    }
    m_discovery_in_process = false;
}

STATIC void central_on_ble_evt(ble_evt_t *ble_evt, void *central_in) {
    bleio_central_obj_t *central = (bleio_central_obj_t*)central_in;

    switch (ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            central->conn_handle = ble_evt->evt.gap_evt.conn_handle;
            central->waiting_to_connect = false;
            break;

        case BLE_GAP_EVT_TIMEOUT:
            // Handle will be invalid.
            central->waiting_to_connect = false;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            central->conn_handle = BLE_CONN_HANDLE_INVALID;
            m_discovery_successful = false;
            m_discovery_in_process = false;
            break;

        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
            on_primary_srv_discovery_rsp(&ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp, central);
            break;

        case BLE_GATTC_EVT_CHAR_DISC_RSP:
            on_char_discovery_rsp(&ble_evt->evt.gattc_evt.params.char_disc_rsp, central);
            break;

        case BLE_GATTC_EVT_DESC_DISC_RSP:
            on_desc_discovery_rsp(&ble_evt->evt.gattc_evt.params.desc_disc_rsp, central);
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            sd_ble_gap_sec_params_reply(central->conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: {
            ble_gap_evt_conn_param_update_request_t *request =
                &ble_evt->evt.gap_evt.params.conn_param_update_request;
            sd_ble_gap_conn_param_update(central->conn_handle, &request->conn_params);
            break;
        }

        default:
            // For debugging.
            // mp_printf(&mp_plat_print, "Unhandled central event: 0x%04x\n", ble_evt->header.evt_id);
            break;
    }
}

void common_hal_bleio_central_construct(bleio_central_obj_t *self) {
    common_hal_bleio_adapter_set_enabled(true);

    self->service_list = mp_obj_new_list(0, NULL);
    self->gatt_role = GATT_ROLE_CLIENT;
    self->conn_handle = BLE_CONN_HANDLE_INVALID;
}

void common_hal_bleio_central_connect(bleio_central_obj_t *self, bleio_address_obj_t *address, mp_float_t timeout, mp_obj_t service_uuids) {
    common_hal_bleio_adapter_set_enabled(true);
    ble_drv_add_event_handler(central_on_ble_evt, self);

    ble_gap_addr_t addr;

    addr.addr_type = address->type;
    mp_buffer_info_t address_buf_info;
    mp_get_buffer_raise(address->bytes, &address_buf_info, MP_BUFFER_READ);
    memcpy(addr.addr, (uint8_t *) address_buf_info.buf, NUM_BLEIO_ADDRESS_BYTES);

    ble_gap_scan_params_t scan_params = {
        .interval = MSEC_TO_UNITS(100, UNIT_0_625_MS),
        .window = MSEC_TO_UNITS(100, UNIT_0_625_MS),
        .scan_phys = BLE_GAP_PHY_1MBPS,
        // timeout of 0 means no timeout
        .timeout = SEC_TO_UNITS(timeout, UNIT_10_MS),
    };

    ble_gap_conn_params_t conn_params = {
        .conn_sup_timeout = MSEC_TO_UNITS(4000, UNIT_10_MS),
        .min_conn_interval = MSEC_TO_UNITS(15, UNIT_1_25_MS),
        .max_conn_interval = MSEC_TO_UNITS(300, UNIT_1_25_MS),
        .slave_latency = 0,          // number of conn events
    };

    self->waiting_to_connect = true;

    uint32_t err_code = sd_ble_gap_connect(&addr, &scan_params, &conn_params, BLE_CONN_CFG_TAG_CUSTOM);

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to start connecting, error 0x%04x"), err_code);
    }

    while (self->waiting_to_connect) {
        MICROPY_VM_HOOK_LOOP;
    }

    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        mp_raise_OSError_msg(translate("Failed to connect: timeout"));
    }

    // Connection successful.
    // Now discover services on the remote peripheral.

    if (service_uuids == mp_const_none) {

        // List of service UUID's not given, so discover all available services.

        uint16_t next_service_start_handle = BLE_GATT_HANDLE_START;

        while (discover_next_services(self, next_service_start_handle, MP_OBJ_NULL)) {
            // discover_next_services() appends to service_list.

            // Get the most recently discovered service, and then ask for services
            // whose handles start after the last attribute handle inside that service.
            const bleio_service_obj_t *service =
                MP_OBJ_TO_PTR(self->service_list->items[self->service_list->len - 1]);
            next_service_start_handle = service->end_handle + 1;
        }
    } else {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(service_uuids, &iter_buf);
        mp_obj_t uuid_obj;
        while ((uuid_obj = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            if (!MP_OBJ_IS_TYPE(uuid_obj, &bleio_uuid_type)) {
                mp_raise_ValueError(translate("non-UUID found in service_uuids"));
            }
            bleio_uuid_obj_t *uuid = MP_OBJ_TO_PTR(uuid_obj);

            ble_uuid_t nrf_uuid;
            bleio_uuid_convert_to_nrf_ble_uuid(uuid, &nrf_uuid);

            // Service might or might not be discovered; that's ok. Caller has to check
            // Central.remote_services to find out.
            // We only need to call this once for each service to discover.
            discover_next_services(self, BLE_GATT_HANDLE_START, &nrf_uuid);
        }
    }


    for (size_t service_idx = 0; service_idx < self->service_list->len; ++service_idx) {
        bleio_service_obj_t *service = MP_OBJ_TO_PTR(self->service_list->items[service_idx]);

        // Skip the service if it had an unknown (unregistered) UUID.
        if (service->uuid == NULL) {
            continue;
        }

        uint16_t next_char_start_handle = service->start_handle;

        // Stop when we go past the end of the range of handles for this service or
        // discovery call returns nothing.
        // discover_next_characteristics() appends to the characteristic_list.
        while (next_char_start_handle <= service->end_handle &&
               discover_next_characteristics(self, service, next_char_start_handle)) {


            // Get the most recently discovered characteristic, and then ask for characteristics
            // whose handles start after the last attribute handle inside that characteristic.
            const bleio_characteristic_obj_t *characteristic =
                MP_OBJ_TO_PTR(service->characteristic_list->items[service->characteristic_list->len - 1]);
            next_char_start_handle = characteristic->handle + 1;
        }

        // Got characteristics for this service. Now discover descriptors for each characteristic.
        size_t char_list_len = service->characteristic_list->len;
        for (size_t char_idx = 0; char_idx < char_list_len; ++char_idx) {
            bleio_characteristic_obj_t *characteristic =
                MP_OBJ_TO_PTR(service->characteristic_list->items[char_idx]);
            const bool last_characteristic = char_idx == char_list_len - 1;
            bleio_characteristic_obj_t *next_characteristic = last_characteristic
                ? NULL
                : MP_OBJ_TO_PTR(service->characteristic_list->items[char_idx + 1]);

            // Skip the characteristic if it had an unknown (unregistered) UUID.
            if (characteristic->uuid == NULL) {
                continue;
            }

            uint16_t next_desc_start_handle = characteristic->handle + 1;

            // Don't run past the end of this service or the beginning of the next characteristic.
            uint16_t next_desc_end_handle = next_characteristic == NULL
                ? service->end_handle
                : next_characteristic->handle - 1;

            // Stop when we go past the end of the range of handles for this service or
            // discovery call returns nothing.
            // discover_next_descriptors() appends to the descriptor_list.
            while (next_desc_start_handle <= service->end_handle &&
                   next_desc_start_handle < next_desc_end_handle &&
                   discover_next_descriptors(self, characteristic,
                                             next_desc_start_handle, next_desc_end_handle)) {

                // Get the most recently discovered descriptor, and then ask for descriptors
                // whose handles start after that descriptor's handle.
                const bleio_descriptor_obj_t *descriptor =
                    MP_OBJ_TO_PTR(characteristic->descriptor_list->items[characteristic->descriptor_list->len - 1]);
                next_desc_start_handle = descriptor->handle + 1;
            }
        }

    }
}

void common_hal_bleio_central_disconnect(bleio_central_obj_t *self) {
    sd_ble_gap_disconnect(self->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}

bool common_hal_bleio_central_get_connected(bleio_central_obj_t *self) {
    return self->conn_handle != BLE_CONN_HANDLE_INVALID;
}

mp_obj_list_t *common_hal_bleio_central_get_remote_services(bleio_central_obj_t *self) {
    return self->service_list;
}
