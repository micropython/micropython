/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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
#include "py/runtime.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "supervisor/shared/bluetooth/bluetooth.h"
#include "common-hal/_bleio/__init__.h"

STATIC conn_state_t conn_state;
osMutexId_t bluetooth_connection_mutex_id;
bleio_adapter_obj_t common_hal_bleio_adapter_obj;

__ALIGNED(4) static uint8_t bluetooth_connection_mutex_cb[osMutexCbSize];
const osMutexAttr_t bluetooth_connection_mutex_attr = {
    .name = "Bluetooth Mutex",
    .attr_bits = osMutexRecursive | osMutexPrioInherit,
    .cb_mem = bluetooth_connection_mutex_cb,
    .cb_size = osMutexCbSize
};

void bleio_user_reset() {
    // Stop any user scanning or advertising.
    common_hal_bleio_adapter_stop_scan(&common_hal_bleio_adapter_obj);
    common_hal_bleio_adapter_stop_advertising(&common_hal_bleio_adapter_obj);

    // Maybe start advertising the BLE workflow.
    supervisor_bluetooth_background();
}

void bleio_reset() {
    reset_dynamic_service();
    reset_packet_buffer_list();
    reset_characteristic_buffer_list();
    bleio_adapter_reset(&common_hal_bleio_adapter_obj);
    // Set this explicitly to save data.
    common_hal_bleio_adapter_obj.base.type = &bleio_adapter_type;
    if (!common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        return;
    }

    supervisor_stop_bluetooth();
    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, false);
    supervisor_start_bluetooth();
}

void bleio_background(void) {

}

void common_hal_bleio_gc_collect(void) {
    bleio_adapter_gc_collect(&common_hal_bleio_adapter_obj);
}

void check_ble_error(int error_code) {
    if (error_code == SL_STATUS_OK) {
        return;
    }
    switch (error_code) {
        case SL_STATUS_TIMEOUT:
            mp_raise_msg(&mp_type_TimeoutError, NULL);
            return;
        default:
            mp_raise_bleio_BluetoothError(
                translate("Unknown BLE error: %d"), error_code);
            break;
    }
}

void common_hal_bleio_check_connected(uint16_t conn_handle) {
    if (conn_handle == BLEIO_HANDLE_INVALID) {
        mp_raise_ConnectionError(translate("Not connected"));
    }
}

// Bluetooth stack event handler.
void sl_bt_on_event(sl_bt_msg_t *evt) {
    sl_status_t sc = SL_STATUS_OK;
    bd_addr address;
    uint8_t address_type = 0;
    STATIC uint8_t serv_idx = 0;
    STATIC uint8_t device_name[16];

    bleio_connection_internal_t *connection;
    bleio_service_obj_t *service;
    bleio_characteristic_obj_t *characteristic;
    bleio_uuid_obj_t *uuid;
    bleio_characteristic_properties_t props;

    switch (SL_BT_MSG_ID(evt->header)) {

        case sl_bt_evt_system_boot_id:

            sc = sl_bt_system_get_identity_address(&address, &address_type);

            snprintf((char *)device_name, 14 + 1,
                "CIRCUITPY-%X%X",address.addr[1], address.addr[0]);
            sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
                0,14,device_name);

            sc = sl_bt_sm_configure(0x00,sl_bt_sm_io_capability_noinputnooutput);
            if (SL_STATUS_OK != sc) {
                mp_raise_bleio_BluetoothError(translate("Sm configure fail"));
            }

            sc = sl_bt_sm_set_bondable_mode(1);
            if (SL_STATUS_OK != sc) {
                mp_raise_bleio_BluetoothError(translate("Set bondable mode fail"));
            }
            sl_bt_sm_delete_bondings();
            break;

        // This event indicates that a new connection was opened.
        case sl_bt_evt_connection_opened_id:
            serv_idx = 0;
            osMutexAcquire(bluetooth_connection_mutex_id, osWaitForever);
            // Device role is Peripheral
            if (evt->data.evt_connection_opened.master == 0) {
                bleio_connections[0].conn_handle =
                    evt->data.evt_connection_opened.connection;
                bleio_connections[0].connection_obj = mp_const_none;
                bleio_connections[0].pair_status = PAIR_PAIRED;
                bleio_connections[0].is_central = false;
                bleio_connections[0].mtu = 0;
            }

            sc = sl_bt_sm_increase_security(
                evt->data.evt_connection_opened.connection);

            if (SL_STATUS_OK != sc) {
                mp_raise_bleio_BluetoothError(
                    translate("Increase security fail."));
            }
            osMutexRelease(bluetooth_connection_mutex_id);
            break;

        case sl_bt_evt_scanner_legacy_advertisement_report_id:

            set_scan_device_info_on_ble_evt(
                evt->data.evt_scanner_legacy_advertisement_report.address,
                evt->data.evt_scanner_legacy_advertisement_report.address_type,
                evt->data.evt_scanner_legacy_advertisement_report.rssi,
                &evt->data.evt_scanner_legacy_advertisement_report.data);

            if (xscan_event != NULL) {
                xEventGroupSetBits(xscan_event,1 << 0);
            }
            break;

        case sl_bt_evt_connection_closed_id:
            common_hal_bleio_adapter_remove_connection(
                evt->data.evt_connection_closed.connection);
            break;

        case sl_bt_evt_system_external_signal_id:
            if (evt->data.evt_system_external_signal.extsignals & 1) {
                sl_bt_external_signal(0);
            }
            break;

        case sl_bt_evt_gatt_service_id:
            osMutexAcquire(bluetooth_connection_mutex_id, osWaitForever);
            connection = bleio_conn_handle_to_connection(
                evt->data.evt_gatt_service.connection);
            service = m_new_obj(bleio_service_obj_t);
            if (NULL == service) {
                mp_raise_bleio_BluetoothError(
                    translate("Create new service obj fail"));
            }
            service->base.type = &bleio_service_type;
            bleio_service_from_connection(service,
                bleio_connection_new_from_internal(connection));
            service->is_remote = true;
            service->handle = evt->data.evt_gatt_service.service;
            uuid = m_new_obj(bleio_uuid_obj_t);
            if (NULL == uuid) {
                osMutexRelease(bluetooth_connection_mutex_id);
                mp_raise_bleio_BluetoothError(
                    translate("Create new service uuid obj fail"));
                break;
            }
            uuid->base.type = &bleio_uuid_type;

            if (UUID16_LEN == evt->data.evt_gatt_service.uuid.len) {
                uuid->efr_ble_uuid.uuid16.value &= 0x0000;
                uuid->efr_ble_uuid.uuid16.value
                    |= evt->data.evt_gatt_service.uuid.data[1];

                uuid->efr_ble_uuid.uuid16.value =
                    (uuid->efr_ble_uuid.uuid16.value << 8)
                    | evt->data.evt_gatt_service.uuid.data[0];
                uuid->efr_ble_uuid.uuid.type = BLE_UUID_TYPE_16;

            } else if (UUID128_LEN == evt->data.evt_gatt_service.uuid.len) {
                memcpy(uuid->efr_ble_uuid.uuid128.value,
                    evt->data.evt_gatt_service.uuid.data, UUID128_LEN);
                uuid->efr_ble_uuid.uuid.type = BLE_UUID_TYPE_128;
            }
            service->uuid = uuid;
            mp_obj_list_append(MP_OBJ_FROM_PTR(connection->remote_service_list),
                MP_OBJ_FROM_PTR(service));
            conn_state = DISCOVER_SERVICES;
            osMutexRelease(bluetooth_connection_mutex_id);
            break;

        case sl_bt_evt_gatt_characteristic_id:
            osMutexAcquire(bluetooth_connection_mutex_id, osWaitForever);
            connection = bleio_conn_handle_to_connection(
                evt->data.evt_gatt_characteristic.connection);
            service =
                MP_OBJ_TO_PTR(connection->remote_service_list->items[serv_idx - 1]);

            characteristic = m_new_obj(bleio_characteristic_obj_t);
            if (characteristic == NULL) {
                mp_raise_bleio_BluetoothError(
                    translate("Create new characteristic obj fail."));
            }

            characteristic->base.type = &bleio_characteristic_type;
            uuid = m_new_obj(bleio_uuid_obj_t);
            if (uuid == NULL) {
                mp_raise_bleio_BluetoothError(
                    translate("Create new characteristic uuid obj fail."));
                break;
            }

            uuid->base.type = &bleio_uuid_type;
            if (UUID16_LEN == evt->data.evt_gatt_characteristic.uuid.len) {
                uuid->efr_ble_uuid.uuid16.value &= 0x0000;
                uuid->efr_ble_uuid.uuid16.value
                    |= evt->data.evt_gatt_characteristic.uuid.data[1];

                uuid->efr_ble_uuid.uuid16.value =
                    (uuid->efr_ble_uuid.uuid16.value << 8)
                    | evt->data.evt_gatt_characteristic.uuid.data[0];

                uuid->efr_ble_uuid.uuid.type = BLE_UUID_TYPE_16;
            } else if (
                UUID128_LEN == evt->data.evt_gatt_characteristic.uuid.len) {

                memcpy(uuid->efr_ble_uuid.uuid128.value,
                    evt->data.evt_gatt_characteristic.uuid.data,
                    UUID128_LEN);
                uuid->efr_ble_uuid.uuid.type = BLE_UUID_TYPE_128;
            }

            props = evt->data.evt_gatt_characteristic.properties;
            characteristic->handle =
                evt->data.evt_gatt_characteristic.characteristic;
            characteristic->props = props;

            common_hal_bleio_characteristic_construct(
                characteristic, service,
                evt->data.evt_gatt_characteristic.characteristic, uuid,
                props, SECURITY_MODE_OPEN, SECURITY_MODE_OPEN,
                0, false,
                mp_const_empty_bytes,
                NULL);

            mp_obj_list_append(MP_OBJ_FROM_PTR(service->characteristic_list),
                MP_OBJ_FROM_PTR(characteristic));
            osMutexRelease(bluetooth_connection_mutex_id);
            break;

        case sl_bt_evt_gatt_procedure_completed_id:

            if (conn_state == DISCOVER_SERVICES
                || conn_state == DISCOVER_CHARACTERISTICS) {

                connection = MP_OBJ_TO_PTR(
                    bleio_conn_handle_to_connection(
                        evt->data.evt_gatt_procedure_completed.connection));

                if (connection != NULL
                    && serv_idx < connection->remote_service_list->len) {

                    service = connection->remote_service_list->items[serv_idx];
                    sc = sl_bt_gatt_discover_characteristics(
                        evt->data.evt_gatt_procedure_completed.connection,
                        service->handle);

                    conn_state = DISCOVER_CHARACTERISTICS;
                    serv_idx++;

                } else {
                    conn_state = RUNNING;
                    serv_idx = 0;
                    if (xdiscovery_event != NULL) {
                        xEventGroupSetBits(xdiscovery_event,1 << 0);
                    }
                }
            }
            break;

        case sl_bt_evt_gatt_characteristic_value_id:

            if (characteristic_buffer_on_ble_evt(
                evt->data.evt_gatt_characteristic_value.characteristic,
                evt->data.evt_gatt_characteristic_value.value.data,
                evt->data.evt_gatt_characteristic_value.value.len)) {
            } else if (packet_buffer_on_ble_evt(
                evt->data.evt_gatt_characteristic_value.characteristic,
                evt->data.evt_gatt_characteristic_value.value.data,
                evt->data.evt_gatt_characteristic_value.value.len)) {
            } else {

                set_characteristic_value_on_ble_evt(
                    evt->data.evt_gatt_characteristic_value.connection,
                    evt->data.evt_gatt_characteristic_value.characteristic,
                    evt->data.evt_gatt_characteristic_value.value.data,
                    evt->data.evt_gatt_characteristic_value.value.len);
            }

            sl_bt_gatt_send_characteristic_confirmation(
                evt->data.evt_gatt_characteristic_value.connection);
            break;

        case sl_bt_evt_gatt_server_attribute_value_id:
            if (characteristic_buffer_on_ble_evt(
                evt->data.evt_gatt_server_attribute_value.attribute,
                evt->data.evt_gatt_server_attribute_value.value.data,
                evt->data.evt_gatt_server_attribute_value.value.len)) {
            } else {
                packet_buffer_on_ble_evt(
                    evt->data.evt_gatt_server_attribute_value.attribute,
                    evt->data.evt_gatt_server_attribute_value.value.data,
                    evt->data.evt_gatt_server_attribute_value.value.len);
            }
            break;

        case sl_bt_evt_gatt_server_characteristic_status_id:
            break;

        case sl_bt_evt_sm_passkey_display_id:
            break;

        case sl_bt_evt_sm_confirm_bonding_id:
            sc = sl_bt_sm_bonding_confirm(
                evt->data.evt_sm_confirm_bonding.connection,1);
            if (SL_STATUS_OK != sc) {
                mp_raise_bleio_BluetoothError(
                    translate("Bonding confirm fail"));
            }
            break;

        case sl_bt_evt_sm_bonded_id:
            break;

        case sl_bt_evt_sm_bonding_failed_id:
            break;

        case sl_bt_evt_connection_parameters_id:
            switch (evt->data.evt_connection_parameters.security_mode)
            {
                case connection_mode1_level1:
                    break;
                case connection_mode1_level2:
                    break;
                case connection_mode1_level3:
                    break;
                case connection_mode1_level4:
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}
