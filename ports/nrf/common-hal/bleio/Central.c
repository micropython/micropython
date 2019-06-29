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
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"

static bleio_service_obj_t *m_char_discovery_service;
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

STATIC void on_primary_srv_discovery_rsp(ble_gattc_evt_prim_srvc_disc_rsp_t *response, bleio_central_obj_t *central) {
    for (size_t i = 0; i < response->count; ++i) {
        ble_gattc_service_t *gattc_service = &response->services[i];

        bleio_service_obj_t *service = m_new_obj(bleio_service_obj_t);
        service->base.type = &bleio_service_type;

        service->device = MP_OBJ_FROM_PTR(central);
        service->characteristic_list = mp_obj_new_list(0, NULL);
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

        if (gattc_char->uuid.type != BLE_UUID_TYPE_UNKNOWN) {
            // Known characteristic UUID.
            bleio_uuid_obj_t *uuid = m_new_obj(bleio_uuid_obj_t);
            uuid->base.type = &bleio_uuid_type;
            bleio_uuid_construct_from_nrf_ble_uuid(uuid, &gattc_char->uuid);
            characteristic->uuid = uuid;
        } else {
            // The discovery response contained a 128-bit UUID that has not yet been registered with the
            // softdevice via sd_ble_uuid_vs_add(). We need to fetch the 128-bit value and register it.
            // For now, just set the UUID to NULL.
            characteristic->uuid = NULL;
        }

        characteristic->props.broadcast = gattc_char->char_props.broadcast;
        characteristic->props.indicate = gattc_char->char_props.indicate;
        characteristic->props.notify = gattc_char->char_props.notify;
        characteristic->props.read = gattc_char->char_props.read;
        characteristic->props.write = gattc_char->char_props.write;
        characteristic->props.write_no_response = gattc_char->char_props.write_wo_resp;
        characteristic->handle = gattc_char->handle_value;
        characteristic->service = m_char_discovery_service;

        mp_obj_list_append(m_char_discovery_service->characteristic_list, MP_OBJ_FROM_PTR(characteristic));
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

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
        sd_ble_gap_sec_params_reply(central->conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
        break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
    {
        ble_gap_evt_conn_param_update_request_t *request = &ble_evt->evt.gap_evt.params.conn_param_update_request;
        sd_ble_gap_conn_param_update(central->conn_handle, &request->conn_params);
        break;
    }
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

        uint16_t next_start_handle = BLE_GATT_HANDLE_START;

        while (discover_next_services(self, next_start_handle, MP_OBJ_NULL)) {
                // discover_next_services() appends to service_list.
                const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(self->service_list);

                // Get the most recently discovered service, and then ask for services
                // whose handles start after the last attribute handle inside that service.
                const bleio_service_obj_t *service = service_list->items[service_list->len - 1];
                next_start_handle = service->end_handle + 1;
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


    const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(self->service_list);
    for (size_t i = 0; i < service_list->len; ++i) {
        bleio_service_obj_t *service = service_list->items[i];

        // Skip the service if it had an unknown (unregistered) UUID.
        if (service->uuid == NULL) {
            continue;
        }

        uint16_t next_start_handle = service->start_handle;

        // Stop when we go past the end of the range of handles for this service or
        // discovery call returns nothing.
        while (next_start_handle <= service->end_handle &&
               discover_next_characteristics(self, service, next_start_handle)) {

            // discover_next_characteristics() appends to the characteristic_list.
            const mp_obj_list_t *characteristic_list = MP_OBJ_TO_PTR(service->characteristic_list);

            // Get the most recently discovered characteristic.
            const bleio_characteristic_obj_t *characteristic =
                characteristic_list->items[characteristic_list->len - 1];
            next_start_handle = characteristic->handle + 1;
        }
    }
}

void common_hal_bleio_central_disconnect(bleio_central_obj_t *self) {
    sd_ble_gap_disconnect(self->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}

mp_obj_t common_hal_bleio_central_get_remote_services(bleio_central_obj_t *self) {
    return self->service_list;
}
