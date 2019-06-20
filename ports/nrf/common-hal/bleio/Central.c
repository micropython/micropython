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
static volatile bool m_discovery_successful;

STATIC bool discover_next_services(bleio_central_obj_t *self, uint16_t start_handle) {
    m_discovery_successful = false;

    uint32_t err_code = sd_ble_gattc_primary_services_discover(self->conn_handle, start_handle, NULL);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to discover services"));
    }

    // Serialize discovery.
    err_code = sd_mutex_acquire(&m_discovery_mutex);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to acquire mutex"));
    }

    // Wait for someone else to release m_discovery_mutex.
    while (sd_mutex_acquire(&m_discovery_mutex) == NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }

    err_code = sd_mutex_release(&m_discovery_mutex);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to release mutex"));
    }

    return m_discovery_successful;
}

STATIC bool discover_next_characteristics(bleio_central_obj_t *self, bleio_service_obj_t *service, uint16_t start_handle) {
    m_char_discovery_service = service;

    ble_gattc_handle_range_t handle_range;
    handle_range.start_handle = start_handle;
    handle_range.end_handle = service->end_handle;

    m_discovery_successful = false;

    uint32_t err_code = sd_ble_gattc_characteristics_discover(self->conn_handle, &handle_range);
    if (err_code != NRF_SUCCESS) {
        return false;
    }

    err_code = sd_mutex_acquire(&m_discovery_mutex);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to acquire mutex"));
    }

    while (sd_mutex_acquire(&m_discovery_mutex) == NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN) {
        MICROPY_VM_HOOK_LOOP;
    }

    err_code = sd_mutex_release(&m_discovery_mutex);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to release mutex"));
    }

    return m_discovery_successful;
}

STATIC void on_primary_srv_discovery_rsp(ble_gattc_evt_prim_srvc_disc_rsp_t *response, bleio_central_obj_t *central) {
    for (size_t i = 0; i < response->count; ++i) {
        ble_gattc_service_t *gattc_service = &response->services[i];

        bleio_service_obj_t *service = m_new_obj(bleio_service_obj_t);
        service->base.type = &bleio_service_type;
        service->device = central;
        service->characteristic_list = mp_obj_new_list(0, NULL);
        service->start_handle = gattc_service->handle_range.start_handle;
        service->end_handle = gattc_service->handle_range.end_handle;
        service->handle = gattc_service->handle_range.start_handle;

        bleio_uuid_obj_t *uuid = m_new_obj(bleio_uuid_obj_t);
        bleio_uuid_construct_from_nrf_ble_uuid(uuid, &gattc_service->uuid);
        service->uuid = uuid;

        mp_obj_list_append(central->service_list, service);
    }

    if (response->count > 0) {
        m_discovery_successful = true;
    }

    const uint32_t err_code = sd_mutex_release(&m_discovery_mutex);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to release mutex"));
    }
}

STATIC void on_char_discovery_rsp(ble_gattc_evt_char_disc_rsp_t *response, bleio_central_obj_t *central) {
    for (size_t i = 0; i < response->count; ++i) {
        ble_gattc_char_t *gattc_char = &response->chars[i];

        bleio_characteristic_obj_t *characteristic = m_new_obj(bleio_characteristic_obj_t);
        characteristic->base.type = &bleio_characteristic_type;

        bleio_uuid_obj_t *uuid = m_new_obj(bleio_uuid_obj_t);
        uuid->base.type = &bleio_uuid_type;
        bleio_uuid_construct_from_nrf_ble_uuid(uuid, &gattc_char->uuid);
        characteristic->uuid = uuid;

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

    const uint32_t err_code = sd_mutex_release(&m_discovery_mutex);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg(translate("Failed to release mutex"));
    }
}

STATIC void on_ble_evt(ble_evt_t *ble_evt, void *central_in) {
    bleio_central_obj_t *central = (bleio_central_obj_t*)central_in;

    switch (ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
    {
        ble_gap_conn_params_t conn_params;
        central->conn_handle = ble_evt->evt.gap_evt.conn_handle;

        sd_ble_gap_ppcp_get(&conn_params);
        sd_ble_gap_conn_param_update(ble_evt->evt.gap_evt.conn_handle, &conn_params);
        break;
    }

    case BLE_GAP_EVT_TIMEOUT:
        if (central->attempting_to_connect) {
            // Signal that connection attempt has timed out.
            central->attempting_to_connect = false;
        }
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        central->conn_handle = BLE_CONN_HANDLE_INVALID;
        break;

    case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
        on_primary_srv_discovery_rsp(&ble_evt->evt.gattc_evt.params.prim_srvc_disc_rsp, central);
        break;

    case BLE_GATTC_EVT_CHAR_DISC_RSP:
        on_char_discovery_rsp(&ble_evt->evt.gattc_evt.params.char_disc_rsp, central);
        break;

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
        sd_ble_gatts_sys_attr_set(ble_evt->evt.gatts_evt.conn_handle, NULL, 0, 0);
        break;

    case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
        sd_ble_gatts_exchange_mtu_reply(central->conn_handle, BLE_GATT_ATT_MTU_DEFAULT);
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

void common_hal_bleio_central_connect(bleio_central_obj_t *self, mp_float_t timeout) {
    common_hal_bleio_adapter_set_enabled(true);
    ble_drv_add_event_handler(on_ble_evt, self);

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

    self->attempting_to_connect = true;

    uint32_t err_code = sd_ble_gap_connect(&scan_params, &m_scan_buffer);

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to start connecting, error 0x%04x"), err_code);
    }

    while (self->conn_handle == BLE_CONN_HANDLE_INVALID && self->attempting_to_connect) {
        #ifdef MICROPY_VM_HOOK_LOOP
        MICROPY_VM_HOOK_LOOP
        #endif
    }

    if (!self->attempting_to_connect) {
        mp_raise_OSError_msg(translate("Failed to connect: timeout"));
    }

    // Conenction successful.
    // Now discover all services on the remote peripheral. Ask for services repeatedly
    // until no more are left.

    uint16_t next_start_handle;

    next_start_handle = BLE_GATT_HANDLE_START;

    while (1) {
        if(!discover_next_services(self, discovery_start_handle)) {
            break;
        }

        // discover_next_services() appends to service_list.
        const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(self->service_list);

        // Get the most recently discovered service, and ask for services with handles
        // starting after the last attribute handle of t
        const bleio_service_obj_t *service = service_list->items[service_list->len - 1];
        next_start_handle = service->end_handle + 1;
    }

    // Now, for each service, discover its characteristics.
    // find characteristics in each service
    const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(self->service_list);
    for (size_t i = 0; i < service_list->len; ++i) {
        bleio_service_obj_t *service = service_list->items[i];

        next_start_handle = service->start_handle;

        while (1) {
            if (!discover_next_characteristics(self, service, service->start_handle)) {
                break;
            }

            // discover_next_characteristics() appends to the characteristic_list.
            const mp_obj_list_t *characteristic_list = MP_OBJ_TO_PTR(service->characteristic_list);

            // Get the most recently discovered characteristic.
            const bleio_characteristic_obj_t *characteristic =
                characteristic_list->items[characteristic_list->len - 1];
            next_start_handle = characteristic->handle + 1;
            if (next_start_handle >= service->end_handle) {
                // Went past the end of the range of handles for this service.
                break;
            }
        }
    }
}

void common_hal_bleio_central_disconnect(bleio_central_obj_t *self) {
    sd_ble_gap_disconnect(self->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}
