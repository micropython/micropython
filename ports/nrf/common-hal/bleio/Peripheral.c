/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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
#include "shared-bindings/bleio/Peripheral.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"

#define BLE_MIN_CONN_INTERVAL        MSEC_TO_UNITS(15, UNIT_0_625_MS)
#define BLE_MAX_CONN_INTERVAL        MSEC_TO_UNITS(300, UNIT_0_625_MS)
#define BLE_SLAVE_LATENCY            0
#define BLE_CONN_SUP_TIMEOUT         MSEC_TO_UNITS(4000, UNIT_10_MS)

#define BLE_ADV_LENGTH_FIELD_SIZE   1
#define BLE_ADV_AD_TYPE_FIELD_SIZE  1
#define BLE_AD_TYPE_FLAGS_DATA_SIZE 1

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

STATIC void check_data_fit(size_t pos, size_t data_len) {
    if (pos + data_len >= BLE_GAP_ADV_SET_DATA_SIZE_MAX) {
        mp_raise_ValueError(translate("Data too large for advertisement packet"));
    }
}

STATIC uint32_t add_services_to_advertisement(bleio_peripheral_obj_t *self, size_t* adv_data_pos_p, size_t uuid_len) {
    uint32_t uuids_total_size = 0;
    const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(self->service_list);
    uint32_t err_code = NRF_SUCCESS;

    check_data_fit(*adv_data_pos_p, 1 + 1);

    // Remember where length byte is; fill in later when we know the size.
    const size_t length_pos = *adv_data_pos_p;
    (*adv_data_pos_p)++;

    self->adv_data[(*adv_data_pos_p)++] = (uuid_len == 16)
        ? BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE
        : BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE;

    for (size_t i = 0; i < service_list->len; ++i) {
        const bleio_service_obj_t *service = MP_OBJ_TO_PTR(service_list->items[i]);
        uint8_t encoded_size = 0;

        // Skip services of the wrong length and secondary services.
        if (common_hal_bleio_uuid_get_size(service->uuid) != uuid_len || service->is_secondary) {
            continue;
        }

        ble_uuid_t uuid;
        bleio_uuid_convert_to_nrf_ble_uuid(service->uuid, &uuid);

        err_code = sd_ble_uuid_encode(&uuid, &encoded_size, &(self->adv_data[*adv_data_pos_p]));
        if (err_code != NRF_SUCCESS) {
            return err_code;
        }

        check_data_fit(*adv_data_pos_p, encoded_size);
        uuids_total_size += encoded_size;
        (*adv_data_pos_p) += encoded_size;
    }

    self->adv_data[length_pos] = 1 + uuids_total_size;  // 1 for the field type.
    return err_code;
}



// if raw_data is a zero-length buffer, generate an advertising packet that advertises the
// services passed in when this Peripheral was created.
// If raw_data contains some bytes, use those bytes as the advertising packet.
// TODO: Generate the advertising packet in Python, not here.
STATIC uint32_t set_advertisement_data(bleio_peripheral_obj_t *self, bool connectable, mp_buffer_info_t *raw_data) {
    common_hal_bleio_adapter_set_enabled(true);

    size_t adv_data_pos = 0;
    uint32_t err_code;

    GET_STR_DATA_LEN(self->name, name_data, name_len);
    if (name_len > 0) {
        ble_gap_conn_sec_mode_t sec_mode;
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

        // We'll add the name after everything else, shortening it if necessary.
        err_code = sd_ble_gap_device_name_set(&sec_mode, name_data, name_len);
        if (err_code != NRF_SUCCESS) {
            return err_code;
        }
    }

    if (raw_data->len != 0) {
        // User-supplied advertising packet.
        check_data_fit(adv_data_pos, raw_data->len);
        memcpy(&(self->adv_data[adv_data_pos]), raw_data->buf, raw_data->len);
        adv_data_pos += raw_data->len;
    } else {
        // Build up advertising packet.
        check_data_fit(adv_data_pos, 1 + 1 + 1);
        self->adv_data[adv_data_pos++] = 2;
        self->adv_data[adv_data_pos++] = BLE_GAP_AD_TYPE_FLAGS;
        self->adv_data[adv_data_pos++] = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

        // The 16-bit ids and 128-bit ids are grouped together by length, so find it whether we have
        // 16 and/or 128-bit service UUIDs.

        const mp_obj_list_t *service_list = MP_OBJ_TO_PTR(self->service_list);
        if (service_list->len > 0) {
            bool has_128bit_services = false;
            bool has_16bit_services = false;

            for (size_t i = 0; i < service_list->len; ++i) {
                const bleio_service_obj_t *service = MP_OBJ_TO_PTR(service_list->items[i]);

                if (service->is_secondary) {
                    continue;
                }

                switch (common_hal_bleio_uuid_get_size(service->uuid)) {
                case 16:
                    has_16bit_services = true;
                    break;
                case 128:
                    has_128bit_services = true;
                    break;
                }
            }

            // Add 16-bit service UUID's in a group, then 128-bit service UUID's.

            if (has_16bit_services) {
                err_code = add_services_to_advertisement(self, &adv_data_pos, 16);
                if (err_code != NRF_SUCCESS) {
                    return err_code;
                }
            }

            if (has_128bit_services) {
                err_code = add_services_to_advertisement(self, &adv_data_pos, 128);
                if (err_code != NRF_SUCCESS) {
                    return err_code;
                }
            }
        }

        // Always include TX power.
        check_data_fit(adv_data_pos, 1 + 1 + 1);
        self->adv_data[adv_data_pos++] = 1 + 1;
        self->adv_data[adv_data_pos++] = BLE_GAP_AD_TYPE_TX_POWER_LEVEL;
        self->adv_data[adv_data_pos++] = 0;   // TODO - allow power level to be set later.

        // We need room for at least a one-character name.
        check_data_fit(adv_data_pos, 1 + 1 + 1);

        // How big a name can we fit?
        size_t bytes_left = BLE_GAP_ADV_SET_DATA_SIZE_MAX - adv_data_pos - 1 - 1;
        size_t partial_name_len = MIN(bytes_left, name_len);
        self->adv_data[adv_data_pos++] = 1 + partial_name_len;
        self->adv_data[adv_data_pos++] = (partial_name_len == name_len)
            ? BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME
            : BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME;
        memcpy(&(self->adv_data[adv_data_pos]), name_data, partial_name_len);
        adv_data_pos += partial_name_len;
    } // end of advertising packet construction

    static ble_gap_adv_params_t m_adv_params = {
        .interval = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
        .properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED,
        .duration = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED,
        .filter_policy = BLE_GAP_ADV_FP_ANY,
        .primary_phy = BLE_GAP_PHY_1MBPS,
    };

    if (!connectable) {
        m_adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
    }

    common_hal_bleio_peripheral_stop_advertising(self);

    const ble_gap_adv_data_t ble_gap_adv_data = {
        .adv_data.p_data = self->adv_data,
        .adv_data.len = adv_data_pos,
    };

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &ble_gap_adv_data, &m_adv_params);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    err_code = sd_ble_gap_adv_start(m_adv_handle, BLE_CONN_CFG_TAG_CUSTOM);

    return err_code;
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
        ble_gap_evt_conn_param_update_request_t *request = &ble_evt->evt.gap_evt.params.conn_param_update_request;
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


void common_hal_bleio_peripheral_construct(bleio_peripheral_obj_t *self) {
    common_hal_bleio_adapter_set_enabled(true);   // TODO -- Do this somewhere else maybe bleio __init__

    self->gatt_role = GATT_ROLE_SERVER;
    self->conn_handle = BLE_CONN_HANDLE_INVALID;

    // Add all the services.

    mp_obj_list_t *service_list = MP_OBJ_TO_PTR(self->service_list);
    for (size_t service_idx = 0; service_idx < service_list->len; ++service_idx) {
        bleio_service_obj_t *service = MP_OBJ_TO_PTR(service_list->items[service_idx]);

        ble_uuid_t uuid;
        bleio_uuid_convert_to_nrf_ble_uuid(service->uuid, &uuid);

        uint8_t service_type = BLE_GATTS_SRVC_TYPE_PRIMARY;
        if (service->is_secondary) {
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


bool common_hal_bleio_peripheral_get_connected(bleio_peripheral_obj_t *self) {
    return self->conn_handle != BLE_CONN_HANDLE_INVALID;
}

void common_hal_bleio_peripheral_start_advertising(bleio_peripheral_obj_t *self, bool connectable, mp_buffer_info_t *raw_data) {
    if (connectable) {
        ble_drv_add_event_handler(peripheral_on_ble_evt, self);
    }

    const uint32_t err_code = set_advertisement_data(self, connectable, raw_data);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to start advertising, err 0x%04x"), err_code);
    }
}

void common_hal_bleio_peripheral_stop_advertising(bleio_peripheral_obj_t *self) {

    if (m_adv_handle == BLE_GAP_ADV_SET_HANDLE_NOT_SET)
        return;

    const uint32_t err_code = sd_ble_gap_adv_stop(m_adv_handle);

    if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_INVALID_STATE)) {
        mp_raise_OSError_msg_varg(translate("Failed to stop advertising, err 0x%04x"), err_code);
    }
}
