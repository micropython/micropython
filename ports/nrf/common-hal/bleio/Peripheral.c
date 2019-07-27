/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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
#include "py/gc.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/bleio/Adapter.h"
#include "shared-bindings/bleio/Characteristic.h"
#include "shared-bindings/bleio/Peripheral.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"
#include "common-hal/bleio/Service.h"

#define BLE_MIN_CONN_INTERVAL        MSEC_TO_UNITS(15, UNIT_0_625_MS)
#define BLE_MAX_CONN_INTERVAL        MSEC_TO_UNITS(300, UNIT_0_625_MS)
#define BLE_SLAVE_LATENCY            0
#define BLE_CONN_SUP_TIMEOUT         MSEC_TO_UNITS(4000, UNIT_10_MS)

#define BLE_ADV_LENGTH_FIELD_SIZE   1
#define BLE_ADV_AD_TYPE_FIELD_SIZE  1
#define BLE_AD_TYPE_FLAGS_DATA_SIZE 1

STATIC void check_data_fit(size_t data_len) {
    if (data_len > BLE_GAP_ADV_SET_DATA_SIZE_MAX) {
        mp_raise_ValueError(translate("Data too large for advertisement packet"));
    }
}

STATIC void peripheral_on_ble_evt(ble_evt_t *ble_evt, void *self_in) {
    bleio_peripheral_obj_t *self = (bleio_peripheral_obj_t*)self_in;

    switch (ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED: {
            // Central has connected.
            ble_gap_conn_params_t conn_params;
            self->conn_handle = ble_evt->evt.gap_evt.conn_handle;
            sd_ble_gap_ppcp_get(&conn_params);
            sd_ble_gap_conn_param_update(ble_evt->evt.gap_evt.conn_handle, &conn_params);
            break;
        }

        case BLE_GAP_EVT_DISCONNECTED:
            // Central has disconnected.
            self->conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            ble_gap_phys_t const phys = {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            sd_ble_gap_phy_update(ble_evt->evt.gap_evt.conn_handle, &phys);
            break;
        }

        case BLE_GAP_EVT_ADV_SET_TERMINATED:
            // Someday may handle timeouts or limit reached.
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            sd_ble_gap_sec_params_reply(self->conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: {
            ble_gap_evt_conn_param_update_request_t *request =
                &ble_evt->evt.gap_evt.params.conn_param_update_request;
            sd_ble_gap_conn_param_update(self->conn_handle, &request->conn_params);
            break;
        }

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
            sd_ble_gap_data_length_update(self->conn_handle, NULL, NULL);
            break;

        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST: {
            sd_ble_gatts_exchange_mtu_reply(self->conn_handle, BLE_GATT_ATT_MTU_DEFAULT);
            break;
        }

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            sd_ble_gatts_sys_attr_set(self->conn_handle, NULL, 0, 0);
            break;

        default:
            // For debugging.
            // mp_printf(&mp_plat_print, "Unhandled peripheral event: 0x%04x\n", ble_evt->header.evt_id);
            break;
    }
}

void common_hal_bleio_peripheral_construct(bleio_peripheral_obj_t *self, mp_obj_list_t *service_list, mp_obj_t name) {
    common_hal_bleio_adapter_set_enabled(true);

    self->service_list = service_list;
    self->name = name;

    self->gatt_role = GATT_ROLE_SERVER;
    self->conn_handle = BLE_CONN_HANDLE_INVALID;
    self->adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

    // Add all the services.

    for (size_t service_idx = 0; service_idx < service_list->len; ++service_idx) {
        bleio_service_obj_t *service = MP_OBJ_TO_PTR(service_list->items[service_idx]);

        service->device = MP_OBJ_FROM_PTR(self);

        ble_uuid_t uuid;
        bleio_uuid_convert_to_nrf_ble_uuid(service->uuid, &uuid);

        uint8_t service_type = BLE_GATTS_SRVC_TYPE_PRIMARY;
        if (common_hal_bleio_service_get_is_secondary(service)) {
            service_type = BLE_GATTS_SRVC_TYPE_SECONDARY;
        }

        const uint32_t err_code = sd_ble_gatts_service_add(service_type, &uuid, &service->handle);
        if (err_code != NRF_SUCCESS) {
            mp_raise_OSError_msg_varg(translate("Failed to add service, err 0x%04x"), err_code);
        }

        // Once the service has been registered, its characteristics can be added.
        common_hal_bleio_service_add_all_characteristics(service);
    }
}


mp_obj_list_t *common_hal_bleio_peripheral_get_service_list(bleio_peripheral_obj_t *self) {
    return self->service_list;
}

bool common_hal_bleio_peripheral_get_connected(bleio_peripheral_obj_t *self) {
    return self->conn_handle != BLE_CONN_HANDLE_INVALID;
}

mp_obj_t common_hal_bleio_peripheral_get_name(bleio_peripheral_obj_t *self) {
    return self->name;
}

void common_hal_bleio_peripheral_start_advertising(bleio_peripheral_obj_t *self, bool connectable, mp_float_t interval, mp_buffer_info_t *advertising_data_bufinfo, mp_buffer_info_t *scan_response_data_bufinfo) {

    // interval value has already been validated.

    if (connectable) {
        ble_drv_add_event_handler(peripheral_on_ble_evt, self);
    }

    common_hal_bleio_adapter_set_enabled(true);

    uint32_t err_code;

    GET_STR_DATA_LEN(self->name, name_data, name_len);
    if (name_len > 0) {
        ble_gap_conn_sec_mode_t sec_mode;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

        err_code = sd_ble_gap_device_name_set(&sec_mode, name_data, name_len);
        if (err_code != NRF_SUCCESS) {
            mp_raise_OSError_msg_varg(translate("Failed to set device name, err 0x%04x"), err_code);

        }
    }

    check_data_fit(advertising_data_bufinfo->len);
    // The advertising data buffers must not move, because the SoftDevice depends on them.
    // So make them long-lived.
    self->advertising_data = (uint8_t *) gc_alloc(BLE_GAP_ADV_SET_DATA_SIZE_MAX * sizeof(uint8_t), false, true);
    memcpy(self->advertising_data, advertising_data_bufinfo->buf, advertising_data_bufinfo->len);

    check_data_fit(scan_response_data_bufinfo->len);
    self->scan_response_data = (uint8_t *) gc_alloc(BLE_GAP_ADV_SET_DATA_SIZE_MAX * sizeof(uint8_t), false, true);
    memcpy(self->scan_response_data, scan_response_data_bufinfo->buf, scan_response_data_bufinfo->len);


    ble_gap_adv_params_t adv_params = {
        .interval = SEC_TO_UNITS(interval, UNIT_0_625_MS),
        .properties.type = connectable ? BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED
        : BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED,
        .duration = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED,
        .filter_policy = BLE_GAP_ADV_FP_ANY,
        .primary_phy = BLE_GAP_PHY_1MBPS,
    };

    common_hal_bleio_peripheral_stop_advertising(self);

    const ble_gap_adv_data_t ble_gap_adv_data = {
        .adv_data.p_data = self->advertising_data,
        .adv_data.len = advertising_data_bufinfo->len,
        .scan_rsp_data.p_data = scan_response_data_bufinfo-> len > 0 ? self->scan_response_data : NULL,
        .scan_rsp_data.len = scan_response_data_bufinfo->len,
    };

    err_code = sd_ble_gap_adv_set_configure(&self->adv_handle, &ble_gap_adv_data, &adv_params);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to configure advertising, err 0x%04x"), err_code);
    }

    err_code = sd_ble_gap_adv_start(self->adv_handle, BLE_CONN_CFG_TAG_CUSTOM);

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to start advertising, err 0x%04x"), err_code);
    }
}

void common_hal_bleio_peripheral_stop_advertising(bleio_peripheral_obj_t *self) {
    if (self->adv_handle == BLE_GAP_ADV_SET_HANDLE_NOT_SET)
        return;

    const uint32_t err_code = sd_ble_gap_adv_stop(self->adv_handle);

    if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_INVALID_STATE)) {
        mp_raise_OSError_msg_varg(translate("Failed to stop advertising, err 0x%04x"), err_code);
    }
}

void common_hal_bleio_peripheral_disconnect(bleio_peripheral_obj_t *self) {
    sd_ble_gap_disconnect(self->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}
