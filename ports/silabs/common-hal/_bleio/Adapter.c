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

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "py/gc.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/ScanEntry.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "common-hal/_bleio/Connection.h"
#include "supervisor/shared/tick.h"

#define PUBLIC_ADDRESS 0
#define STATIC_ADDRESS 1
#define BLE_EXT_ADV_MAX_SIZE 32

#define UNIT_0_625_MS (625)
#define UNIT_1_25_MS (1250)
#define UNIT_10_MS (10000)

#define SCAN_TIMEOUT_MS_DEFAUT 3000
#define BLE_GAP_SCAN_BUFFER_EXTENDED_MAX_SUPPORTED 256
#define BLE_GAP_SCAN_BUFFER_MAX 32
#define DEVICE_NAME_LEN 15

EventGroupHandle_t xscan_event;
device_scan_info_t scan_info;
bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

// Set scan data form sl_bt_on_event
void set_scan_device_info_on_ble_evt(bd_addr address,
    uint8_t address_type,
    int8_t rssi,
    uint8array *data) {
    scan_info.address = address;
    scan_info.address_type = address_type;
    scan_info.rssi = rssi;
    memcpy(scan_info.data, data->data, data->len);
}

// Get state of the BLE adapter.
bool common_hal_bleio_adapter_get_enabled(bleio_adapter_obj_t *self) {
    return self->is_enable;
}

// Set state of the BLE adapter
void common_hal_bleio_adapter_set_enabled(bleio_adapter_obj_t *self,
    bool enabled) {

    const bool is_enabled = common_hal_bleio_adapter_get_enabled(self);
    bd_addr get_address;
    uint8_t address_type;
    uint8_t conn_index;
    bleio_connection_internal_t *connection;
    sl_status_t sc = SL_STATUS_FAIL;
    uint8_t device_name[DEVICE_NAME_LEN + 1];
    memset(device_name, 0, DEVICE_NAME_LEN);

    // Don't enable or disable twice
    if (is_enabled == enabled) {
        return;
    }

    sc = sl_bt_system_get_identity_address(&get_address, &address_type);
    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(translate("Get address fail."));
    }
    snprintf((char *)device_name, DEVICE_NAME_LEN + 1,
        "CIRCUITPY-%X%X", get_address.addr[1], get_address.addr[0]);

    if (enabled) {
        sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
            0,
            DEVICE_NAME_LEN,
            device_name);

        // Clear all of the internal connection objects.
        for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
            connection = &bleio_connections[conn_index];
            // Reset connection.
            connection->conn_handle = BLEIO_HANDLE_INVALID;
        }
    } else {
        self->is_enable = false;
    }
}

// Get mac address of the BLE adapter
bleio_address_obj_t *common_hal_bleio_adapter_get_address(bleio_adapter_obj_t *self) {
    bd_addr get_address;
    uint8_t address_type;
    sl_status_t sc = SL_STATUS_FAIL;
    bleio_address_obj_t *address;

    sc = sl_bt_system_get_identity_address(&get_address, &address_type);
    if (SL_STATUS_OK != sc) {
        return NULL;
    }

    address = mp_obj_malloc(bleio_address_obj_t, &bleio_address_type);
    common_hal_bleio_address_construct(address, get_address.addr,
        BLEIO_ADDRESS_TYPE_RANDOM_STATIC);
    return address;
}

// Set identity address
bool common_hal_bleio_adapter_set_address(bleio_adapter_obj_t *self,
    bleio_address_obj_t *address) {

    sl_status_t sc = SL_STATUS_FAIL;
    mp_buffer_info_t bufinfo;
    bd_addr ble_addr;

    if (NULL == address) {
        return false;
    }
    if (!mp_get_buffer(address->bytes, &bufinfo, MP_BUFFER_READ)) {
        return false;
    }
    memcpy(ble_addr.addr, bufinfo.buf, 6);
    sl_bt_system_set_identity_address(ble_addr, PUBLIC_ADDRESS);
    return sc == SL_STATUS_OK;
}

// Get name of the BLE adapter
mp_obj_str_t *common_hal_bleio_adapter_get_name(bleio_adapter_obj_t *self) {
    char name[DEVICE_NAME_LEN];
    size_t value_len = 0;
    uint8_t sc;
    memset(name, 0, DEVICE_NAME_LEN);
    sc = sl_bt_gatt_server_read_attribute_value(gattdb_device_name,
        0,
        DEVICE_NAME_LEN,
        &value_len,
        (uint8_t *)name);
    if (SL_STATUS_OK != sc) {
        return NULL;
    }
    return mp_obj_new_str(name, strlen(name));
}

// Set name of the BLE adapter
void common_hal_bleio_adapter_set_name(bleio_adapter_obj_t *self,
    const char *name) {
    sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
        0,
        DEVICE_NAME_LEN,
        (const uint8_t *)name);
}

// starts a BLE scan
mp_obj_t common_hal_bleio_adapter_start_scan(bleio_adapter_obj_t *self,
    uint8_t *prefixes,
    size_t prefix_length,
    bool extended,
    mp_int_t buffer_size,
    mp_float_t timeout,
    mp_float_t interval,
    mp_float_t window,
    mp_int_t minimum_rssi,
    bool active) {

    sl_status_t sc;
    uint64_t start_ticks = supervisor_ticks_ms64();
    uint64_t current_ticks = start_ticks;
    uint32_t timeout_ms = timeout * 1000;

    if (timeout_ms == 0) {
        timeout_ms = SCAN_TIMEOUT_MS_DEFAUT;
    }

    if (self->scan_results != NULL) {
        if (!shared_module_bleio_scanresults_get_done(self->scan_results)) {
            mp_raise_bleio_BluetoothError(
                translate("Scan already in progress. Stop with stop_scan."));
        }
        self->scan_results = NULL;
    }

    sl_bt_scanner_stop();
    self->scan_results = shared_module_bleio_new_scanresults(buffer_size,
        prefixes,
        prefix_length,
        minimum_rssi);
    xscan_event = xEventGroupCreate();
    if (xscan_event != NULL) {
        xEventGroupClearBits(xscan_event, 1 << 0);
    }

    sc = sl_bt_scanner_start(sl_bt_scanner_scan_phy_1m,
        sl_bt_scanner_discover_generic);

    if (SL_STATUS_OK != sc) {
        self->scan_results = NULL;
    }

    // Busy-wait until timeout or until we've read enough chars.
    while (current_ticks - start_ticks <= timeout_ms) {
        if (xscan_event != NULL) {
            xEventGroupWaitBits(xscan_event, 1 << 0, pdTRUE, pdFALSE,
                    timeout_ms / portTICK_PERIOD_MS);
        }
        self->scan_results->prefix_length = 0;
        scan_info.data_len = 28;
        shared_module_bleio_scanresults_append(
            self->scan_results,
            supervisor_ticks_ms64(),
            true,
            true,
            scan_info.rssi,
            scan_info.address.addr,
            scan_info.address_type,
            scan_info.data,
            scan_info.data_len);
        current_ticks = supervisor_ticks_ms64();
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            break;
        }
    }

    shared_module_bleio_scanresults_set_done(self->scan_results, true);
    vEventGroupDelete(xscan_event);
    return MP_OBJ_FROM_PTR(self->scan_results);
}

void common_hal_bleio_adapter_stop_scan(bleio_adapter_obj_t *self) {
    if (self->scan_results == NULL) {
        return;
    }
    sl_bt_scanner_stop();
    shared_module_bleio_scanresults_set_done(self->scan_results, true);
    self->scan_results = NULL;
}

// Start the  advertising on an advertising set with specified
// discovery and connection modes
uint32_t _common_hal_bleio_adapter_start_advertising(
    bleio_adapter_obj_t *self,
    bool connectable,
    bool anonymous,
    uint32_t timeout,
    float interval,
    const uint8_t *advertising_data,
    uint16_t advertising_data_len,
    const uint8_t *scan_response_data,
    uint16_t scan_response_data_len,
    mp_int_t tx_power,
    const bleio_address_obj_t *directed_to) {

    sl_status_t sc = SL_STATUS_FAIL;
    int16_t power = tx_power * 10; // TX power in 0.1 dBm steps
    int16_t set_power;
    uint32_t interval_min = (uint32_t)(interval * 1600);  // (milliseconds * 1.6)
    uint32_t interval_max = (uint32_t)(interval * 1600);  // (milliseconds * 1.6)
    bd_addr address;
    uint8_t address_type;
    uint8_t system_id[8];
    uint8_t enable_connect;

    if (self->user_advertising) {
        return SL_STATUS_BUSY;
    }

    sc = sl_bt_advertiser_create_set(&self->advertising_handle);

    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(translate("Create_set fail."));
        return sc;
    }
    sc = sl_bt_advertiser_set_channel_map(self->advertising_handle, 7);
    if (SL_STATUS_OK != sc) {
        return sc;
    }

    sc = sl_bt_system_get_identity_address(&address, &address_type);

    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(translate("Get address fail."));
        return sc;
    }
    // Pad and reverse unique ID to get System ID.
    system_id[0] = address.addr[5];
    system_id[1] = address.addr[4];
    system_id[2] = address.addr[3];
    system_id[3] = 0xFF;
    system_id[4] = 0xFE;
    system_id[5] = address.addr[2];
    system_id[6] = address.addr[1];
    system_id[7] = address.addr[0];

    sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
        0,
        sizeof(system_id),
        system_id);
    if (SL_STATUS_OK != sc) {
        return sc;
    }

    sc = sl_bt_advertiser_set_tx_power(self->advertising_handle,
        power,
        &set_power);
    if (SL_STATUS_OK != sc) {
        return sc;
    }

    // Set advertising interval.
    sc = sl_bt_advertiser_set_timing(
        self->advertising_handle,
        interval_min,  // min. adv. interval (milliseconds * 1.6)
        interval_max,  // max. adv. interval (milliseconds * 1.6)
        0,             // adv. duration
        0);            // max. num. adv. events

    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(translate("Set_timing fail."));
        return sc;
    }

    sc = sl_bt_legacy_advertiser_set_data(
        self->advertising_handle,
        sl_bt_advertiser_advertising_data_packet,
        advertising_data_len,
        advertising_data);

    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(translate("Set data fail."));
        return sc;
    }

    // Start advertising and enable connections.
    enable_connect = sl_bt_legacy_advertiser_scannable;
    if (true == connectable) {
        enable_connect = sl_bt_advertiser_connectable_scannable;
    }
    sc = sl_bt_legacy_advertiser_start(self->advertising_handle,
        enable_connect);

    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(translate("Start advertise fail."));
    } else {
        self->user_advertising = true;
    }

    return sc;
}

// Check size of packet advertising to send
STATIC void check_data_fit(size_t data_len, bool connectable) {
    if (data_len > BLE_EXT_ADV_MAX_SIZE ||
        (connectable && data_len > BLE_EXT_ADV_MAX_SIZE)) {
        mp_raise_ValueError(
            translate("Data too large for advertisement packet"));
    }
}

// Start advertising
void common_hal_bleio_adapter_start_advertising(
    bleio_adapter_obj_t *self,
    bool connectable,
    bool anonymous,
    uint32_t timeout,
    mp_float_t interval,
    mp_buffer_info_t *advertising_data_bufinfo,
    mp_buffer_info_t *scan_response_data_bufinfo,
    mp_int_t tx_power,
    const bleio_address_obj_t *directed_to) {

    if (self->user_advertising) {
        common_hal_bleio_adapter_stop_advertising(self);
    }
    // Interval value has already been validated.

    check_data_fit(advertising_data_bufinfo->len, connectable);
    check_data_fit(scan_response_data_bufinfo->len, connectable);

    if (advertising_data_bufinfo->len > 31 && scan_response_data_bufinfo->len > 0) {
        mp_raise_bleio_BluetoothError(
            translate("Extended advertisements not supported"));
    }

    if (advertising_data_bufinfo->len > 0 && directed_to != NULL) {
        mp_raise_bleio_BluetoothError(
            translate("Data not supported with directed advertising"));
    }

    if (anonymous) {
        mp_raise_NotImplementedError(NULL);
    }

    if (!timeout) {
        timeout = INT32_MAX;
    } else if (timeout > INT32_MAX) {
        mp_raise_bleio_BluetoothError(
            translate("Maximum timeout length is %d seconds"), INT32_MAX / 1000);
    }

    _common_hal_bleio_adapter_start_advertising(self, connectable, anonymous,
        timeout, interval,
        advertising_data_bufinfo->buf,
        advertising_data_bufinfo->len,
        scan_response_data_bufinfo->buf,
        scan_response_data_bufinfo->len,
        tx_power,
        directed_to);
}

// Stop advertising
void common_hal_bleio_adapter_stop_advertising(bleio_adapter_obj_t *self) {
    if (!common_hal_bleio_adapter_get_advertising(self)) {
        return;
    }
    sl_bt_advertiser_delete_set(self->advertising_handle);
    sl_bt_advertiser_stop(0);
    self->user_advertising = false;
}

// Get status of advertising
bool common_hal_bleio_adapter_get_advertising(bleio_adapter_obj_t *self) {
    return self->user_advertising;
}

// Convert mac address of remote device to connect
STATIC void _convert_address(const bleio_address_obj_t *address,
    bd_addr *sd_address, uint8_t *addr_type) {
    mp_buffer_info_t address_buf_info;
    *addr_type = address->type;
    mp_get_buffer_raise(address->bytes, &address_buf_info, MP_BUFFER_READ);
    memcpy(sd_address->addr, (uint8_t *)address_buf_info.buf, 6);
}

// Add new connection into connection list
void _new_connection(uint16_t conn_handle) {
    // Find an empty connection. One must always be available because the SD has the same
    // total connection limit.
    bleio_connection_internal_t *connection;
    uint8_t conn_index;
    for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
        connection = &bleio_connections[conn_index];
        if (connection->conn_handle == BLEIO_HANDLE_INVALID) {
            break;
        }
    }
    connection->conn_handle = conn_handle;
    connection->connection_obj = mp_const_none;
    connection->pair_status = PAIR_NOT_PAIRED;
    connection->mtu = 0;
}

// Attempts a connection to the device with the given address.
mp_obj_t common_hal_bleio_adapter_connect(bleio_adapter_obj_t *self,
    bleio_address_obj_t *address,
    mp_float_t timeout) {

    bd_addr addr;
    uint8_t address_type;
    sl_status_t error_code;
    uint8_t conn_handle;
    uint8_t conn_index;
    bleio_connection_internal_t *connection;

    if (self->scan_results != NULL) {
        common_hal_bleio_adapter_stop_scan(self);
    }
    conn_handle = common_hal_bleio_adapter_get_connected(self);
    timeout = MSEC_TO_UNITS(timeout, UNIT_10_MS);

    _convert_address(address, &addr, &address_type);

    error_code = sl_bt_connection_open(addr,
        address_type,
        sl_bt_gap_phy_1m,
        &conn_handle);

    // Negative values are error codes, connection handle otherwise.
    if (SL_STATUS_OK != error_code) {
        return mp_const_none;
    }

    _new_connection(conn_handle);

    // TODO: If we have keys, then try and encrypt the connection.

    // TODO: Negotiate for better PHY and data lengths since we are the central. These are
    // nice-to-haves so ignore any errors.

    // Make the connection object and return it.
    for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
        connection = &bleio_connections[conn_index];
        if (connection->conn_handle == conn_handle) {
            connection->is_central = true;
            connection->pair_status = PAIR_PAIRED;
            return bleio_connection_new_from_internal(connection);
        }
    }

    mp_raise_bleio_BluetoothError(
        translate("Failed to connect: internal error"));
    return mp_const_none;
}

// Get connected status
bool common_hal_bleio_adapter_get_connected(bleio_adapter_obj_t *self) {
    uint8_t conn_index;
    bleio_connection_internal_t *connection;
    for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
        connection = &bleio_connections[conn_index];
        if (connection->conn_handle != BLEIO_HANDLE_INVALID) {
            return true;
        }
    }
    return false;
}

// Get connection object
mp_obj_t common_hal_bleio_adapter_get_connections(bleio_adapter_obj_t *self) {

    size_t total_connected = 0;
    mp_obj_t items[BLEIO_TOTAL_CONNECTION_COUNT];
    uint8_t conn_index;
    bleio_connection_internal_t *connection;

    if (self->connection_objs != NULL) {
        return self->connection_objs;
    }
    for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
        connection = &bleio_connections[conn_index];
        if (connection->conn_handle != BLEIO_HANDLE_INVALID) {
            if (connection->connection_obj == mp_const_none) {
                connection->connection_obj =
                    bleio_connection_new_from_internal(connection);
            }
            items[total_connected] = connection->connection_obj;
            total_connected++;
        }
    }
    self->connection_objs = mp_obj_new_tuple(total_connected, items);
    return self->connection_objs;
}

void common_hal_bleio_adapter_remove_connection(uint8_t conn_handle) {
    uint8_t conn_index;
    bleio_connection_internal_t *connection;
    osMutexAcquire(bluetooth_connection_mutex_id, osWaitForever);
    for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
        connection = &bleio_connections[conn_index];
        if (conn_handle == connection->conn_handle) {
            connection->connection_obj = NULL;
            connection->conn_handle = BLEIO_HANDLE_INVALID;
        }
    }
    osMutexRelease(bluetooth_connection_mutex_id);
}

// Delete all bonding
void common_hal_bleio_adapter_erase_bonding(bleio_adapter_obj_t *self) {
    sl_status_t sc;
    sc = sl_bt_sm_delete_bondings();
    if (SL_STATUS_OK != sc) {
        mp_raise_bleio_BluetoothError(translate("All bonding deleted fail."));
    }
}

// Get status bonding to central
bool common_hal_bleio_adapter_is_bonded_to_central(bleio_adapter_obj_t *self) {
    bleio_connection_internal_t *connection;
    uint8_t conn_index;
    for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
        connection = &bleio_connections[conn_index];
        if (connection->conn_handle != BLEIO_HANDLE_INVALID) {
            return true;
        }
    }
    return false;
}

void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter) {
    // We divide by size_t so that we can scan each 32-bit aligned value to see
    // if it is a pointer. This allows us to change the structs without worrying
    // about collecting new pointers.
    gc_collect_root((void **)adapter,
        sizeof(bleio_adapter_obj_t) / (sizeof(size_t)));
    gc_collect_root((void **)bleio_connections,
        sizeof(bleio_connections) / (sizeof(size_t)));
}

// Reset the BLE adapter
void bleio_adapter_reset(bleio_adapter_obj_t *adapter) {

    bool any_connected = false;
    uint64_t start_ticks;
    uint8_t conn_index;
    bleio_connection_internal_t *connection;

    common_hal_bleio_adapter_stop_scan(adapter);
    if (common_hal_bleio_adapter_get_advertising(adapter)) {
        common_hal_bleio_adapter_stop_advertising(adapter);
    }

    adapter->connection_objs = NULL;
    for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
        connection = &bleio_connections[conn_index];
        // Disconnect all connections cleanly.
        if (connection->conn_handle != BLEIO_HANDLE_INVALID) {
            common_hal_bleio_connection_disconnect(connection);
        }
        connection->connection_obj = mp_const_none;
    }

    // Wait up to 125 ms (128 ticks) for disconnect to complete. This should be
    // greater than most connection intervals.
    start_ticks = supervisor_ticks_ms64();
    while (any_connected && supervisor_ticks_ms64() - start_ticks < 128) {
        any_connected = false;
        for (conn_index = 0; conn_index < BLEIO_TOTAL_CONNECTION_COUNT; conn_index++) {
            connection = &bleio_connections[conn_index];
            any_connected |= connection->conn_handle != BLEIO_HANDLE_INVALID;
        }
    }
}
