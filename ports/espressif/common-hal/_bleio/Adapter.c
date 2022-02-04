/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "py/gc.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "supervisor/shared/bluetooth/bluetooth.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/tick.h"
#include "supervisor/usb.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/nvm/ByteArray.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/ScanEntry.h"
#include "shared-bindings/time/__init__.h"

#include "controller/ble_ll_adv.h"
#include "nimble/hci_common.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_gap.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

#include "common-hal/_bleio/Connection.h"

#include "esp_bt.h"
#include "esp_nimble_hci.h"

bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

// static void bluetooth_adapter_background(void *data) {
//     supervisor_bluetooth_background();
//     bleio_background();
// }

bool ble_active = false;

static void nimble_host_task(void *param) {
    nimble_port_run();
    nimble_port_freertos_deinit();
}

static TaskHandle_t cp_task = NULL;

static void _on_sync(void) {
    int rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    xTaskNotifyGive(cp_task);
}

void common_hal_bleio_adapter_set_enabled(bleio_adapter_obj_t *self, bool enabled) {
    const bool is_enabled = common_hal_bleio_adapter_get_enabled(self);

    // Don't enable or disable twice
    if (is_enabled == enabled) {
        return;
    }

    if (enabled) {
        esp_nimble_hci_and_controller_init();
        nimble_port_init();
        // ble_hs_cfg.reset_cb = blecent_on_reset;
        ble_hs_cfg.sync_cb = _on_sync;
        // ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
        ble_svc_gap_device_name_set("CIRCUITPY");

        // Clear all of the internal connection objects.
        for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
            bleio_connection_internal_t *connection = &bleio_connections[i];
            // Reset connection.
            connection->conn_handle = BLEIO_HANDLE_INVALID;
        }

        cp_task = xTaskGetCurrentTaskHandle();

        nimble_port_freertos_init(nimble_host_task);
        // Wait for sync.
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(200));
    } else {
        nimble_port_stop();
    }
}

bool common_hal_bleio_adapter_get_enabled(bleio_adapter_obj_t *self) {
    return xTaskGetHandle("ble") != NULL;
}

bleio_address_obj_t *common_hal_bleio_adapter_get_address(bleio_adapter_obj_t *self) {
    uint8_t address_bytes[6];
    uint8_t address_type = BLE_ADDR_RANDOM;
    ble_hs_id_infer_auto(0, &address_type);
    int result = ble_hs_id_copy_addr(address_type, address_bytes, NULL);
    if (result != 0) {
        return NULL;
    }

    bleio_address_obj_t *address = m_new_obj(bleio_address_obj_t);
    address->base.type = &bleio_address_type;
    common_hal_bleio_address_construct(address, address_bytes, BLEIO_ADDRESS_TYPE_RANDOM_STATIC);
    return address;
}

bool common_hal_bleio_adapter_set_address(bleio_adapter_obj_t *self, bleio_address_obj_t *address) {
    if (address->type != BLEIO_ADDRESS_TYPE_RANDOM_STATIC) {
        return false;
    }
    mp_buffer_info_t bufinfo;
    if (!mp_get_buffer(address->bytes, &bufinfo, MP_BUFFER_READ)) {
        return false;
    }
    int result = ble_hs_id_set_rnd(bufinfo.buf);
    return result == 0;
}

mp_obj_str_t *common_hal_bleio_adapter_get_name(bleio_adapter_obj_t *self) {
    const char *name = ble_svc_gap_device_name();

    return mp_obj_new_str(name, strlen(name));
}

void common_hal_bleio_adapter_set_name(bleio_adapter_obj_t *self, const char *name) {
    ble_svc_gap_device_name_set(name);
}

static int _scan_event(struct ble_gap_event *event, void *scan_results_in) {
    bleio_scanresults_obj_t *scan_results = (bleio_scanresults_obj_t *)scan_results_in;

    if (event->type == BLE_GAP_EVENT_DISC_COMPLETE) {
        shared_module_bleio_scanresults_set_done(scan_results, true);
        return 0;
    }

    if (event->type != BLE_GAP_EVENT_DISC && event->type != BLE_GAP_EVENT_EXT_DISC) {
        #if CIRCUITPY_VERBOSE_BLE
        mp_printf(&mp_plat_print, "Unsupported scan event %d\n", event->type);
        #endif
        return 0;
    }

    if (event->type == BLE_GAP_EVENT_DISC) {
        // Legacy advertisement
        struct ble_gap_disc_desc *disc = &event->disc;
        bool connectable = disc->event_type == BLE_HCI_ADV_RPT_EVTYPE_ADV_IND ||
            disc->event_type == BLE_HCI_ADV_RPT_EVTYPE_DIR_IND;
        shared_module_bleio_scanresults_append(scan_results,
            supervisor_ticks_ms64(),
            connectable,
            disc->event_type == BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP,
            disc->rssi,
            disc->addr.val,
            disc->addr.type,
            disc->data,
            disc->length_data);
    } else {
        // Extended advertisement
        struct ble_gap_ext_disc_desc *disc = &event->ext_disc;
        shared_module_bleio_scanresults_append(scan_results,
            supervisor_ticks_ms64(),
            (disc->props & BLE_HCI_ADV_CONN_MASK) != 0,
            (disc->props & BLE_HCI_ADV_SCAN_MASK) != 0,
            disc->rssi,
            disc->addr.val,
            disc->addr.type,
            disc->data,
            disc->length_data);
    }

    return 0;
}

mp_obj_t common_hal_bleio_adapter_start_scan(bleio_adapter_obj_t *self, uint8_t *prefixes,
    size_t prefix_length, bool extended, mp_int_t buffer_size, mp_float_t timeout,
    mp_float_t interval, mp_float_t window, mp_int_t minimum_rssi, bool active) {
    if (self->scan_results != NULL) {
        if (!shared_module_bleio_scanresults_get_done(self->scan_results)) {
            mp_raise_bleio_BluetoothError(translate("Scan already in progess. Stop with stop_scan."));
        }
        self->scan_results = NULL;
    }

    self->scan_results = shared_module_bleio_new_scanresults(buffer_size, prefixes, prefix_length, minimum_rssi);
    // size_t max_packet_size = extended ? BLE_HCI_MAX_EXT_ADV_DATA_LEN : BLE_HCI_MAX_ADV_DATA_LEN;

    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params;

    /* Figure out address to use while advertising (no privacy for now) */
    int privacy = 0;
    CHECK_NIMBLE_ERROR(ble_hs_id_infer_auto(privacy, &own_addr_type));

    disc_params.filter_duplicates = 0;
    disc_params.passive = !active;
    disc_params.itvl = interval / 0.625;
    disc_params.window = window / 0.625;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    size_t duration_ms = timeout * 1000;
    if (duration_ms == 0) {
        duration_ms = BLE_HS_FOREVER;
    }

    CHECK_NIMBLE_ERROR(ble_gap_disc(own_addr_type, duration_ms, &disc_params,
        _scan_event, self->scan_results));

    return MP_OBJ_FROM_PTR(self->scan_results);
}

void common_hal_bleio_adapter_stop_scan(bleio_adapter_obj_t *self) {
    if (self->scan_results == NULL) {
        return;
    }
    ble_gap_disc_cancel();
    shared_module_bleio_scanresults_set_done(self->scan_results, true);
    self->scan_results = NULL;
}

STATIC void _convert_address(const bleio_address_obj_t *address, ble_addr_t *nimble_address) {
    nimble_address->type = address->type;
    mp_buffer_info_t address_buf_info;
    mp_get_buffer_raise(address->bytes, &address_buf_info, MP_BUFFER_READ);
    memcpy(nimble_address->val, (uint8_t *)address_buf_info.buf, NUM_BLEIO_ADDRESS_BYTES);
}

STATIC void _new_connection(uint16_t conn_handle) {
    // Set the tx_power for the connection higher than the advertisement.
    esp_ble_tx_power_set(conn_handle, ESP_PWR_LVL_N0);


    // Find an empty connection. One must always be available because the SD has the same
    // total connection limit.
    bleio_connection_internal_t *connection;
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        connection = &bleio_connections[i];
        if (connection->conn_handle == BLEIO_HANDLE_INVALID) {
            break;
        }
    }
    connection->conn_handle = conn_handle;
    connection->connection_obj = mp_const_none;
    connection->pair_status = PAIR_NOT_PAIRED;
    connection->mtu = 0;

    // Change the callback for the connection.
    ble_gap_set_event_cb(conn_handle, bleio_connection_event_cb, connection);
}

mp_obj_t common_hal_bleio_adapter_connect(bleio_adapter_obj_t *self, bleio_address_obj_t *address, mp_float_t timeout) {
    mp_raise_NotImplementedError(NULL);

    mp_raise_bleio_BluetoothError(translate("Failed to connect: internal error"));

    return mp_const_none;
}

typedef struct {
    struct os_mbuf mbuf;
    struct os_mbuf_pkthdr hdr;
} os_mbuf_t;

STATIC void _wrap_in_mbuf(const uint8_t *data, uint16_t len, os_mbuf_t *buf) {
    struct os_mbuf *mbuf = &buf->mbuf;
    mbuf->om_data = (uint8_t *)data,
    mbuf->om_flags = 0;
    mbuf->om_pkthdr_len = 0;
    mbuf->om_len = len;
    mbuf->om_next.sle_next = NULL;

    buf->hdr.omp_len = len;

    // Setting the pool to NULL will cause frees to fail. Hopefully that failure
    // is ignored.
    mbuf->om_omp = NULL;
}

static int _advertising_event(struct ble_gap_event *event, void *self_in) {
    bleio_adapter_obj_t *self = (bleio_adapter_obj_t *)self_in;

    #if CIRCUITPY_VERBOSE_BLE
    mp_printf(&mp_plat_print, "Advertising event: %d\n", event->type);
    #endif
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            // Spurious connect events can happen.
            break;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            if (event->adv_complete.reason == NIMBLE_OK) {
                _new_connection(event->adv_complete.conn_handle);
                // Set connections objs back to NULL since we have a new
                // connection and need a new tuple.
                self->connection_objs = NULL;
            }
            // Other statuses indicate timeout or preemption.
            common_hal_bleio_adapter_stop_advertising(self);
            break;

        default:
            #if CIRCUITPY_VERBOSE_BLE
            // For debugging.
            mp_printf(&mp_plat_print, "Unhandled advertising event: %d\n", event->type);
            #endif
            break;
    }
    return 0;
}

uint32_t _common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self,
    bool connectable, bool anonymous, uint32_t timeout, float interval,
    const uint8_t *advertising_data, uint16_t advertising_data_len,
    const uint8_t *scan_response_data, uint16_t scan_response_data_len,
    mp_int_t tx_power, const bleio_address_obj_t *directed_to) {

    if (ble_gap_adv_active() && !self->user_advertising) {
        return BLE_HS_EBUSY;
    }

    uint32_t rc;
    bool extended = advertising_data_len > BLE_ADV_LEGACY_DATA_MAX_LEN ||
        scan_response_data_len > BLE_ADV_LEGACY_DATA_MAX_LEN;

    ble_addr_t peer;
    if (directed_to != NULL) {
        _convert_address(directed_to, &peer);
    }

    uint8_t own_addr_type;
    // Anonymous addresses are still resolvable. (Following what the NRF
    // implementation does.)
    rc = ble_hs_id_infer_auto(anonymous, &own_addr_type);
    if (rc != NIMBLE_OK) {
        return rc;
    }

    bool high_duty_directed = directed_to != NULL && interval <= 3.5 && timeout <= 1.3;

    struct ble_gap_ext_adv_params adv_params = {
        .connectable = connectable,
        .scannable = scan_response_data_len > 0,
        .directed = directed_to != NULL,
        .high_duty_directed = high_duty_directed,
        .legacy_pdu = !extended,
        .anonymous = anonymous,
        .include_tx_power = extended,
        .scan_req_notif = false,
        .itvl_min = SEC_TO_UNITS(interval, UNIT_0_625_MS) + 0.5f,
        .itvl_max = SEC_TO_UNITS(interval, UNIT_0_625_MS) + 0.5f,
        .channel_map = 0,
        .own_addr_type = own_addr_type,
        .peer = peer,
        .filter_policy = BLE_HCI_CONN_FILT_NO_WL,
        .primary_phy = BLE_HCI_LE_PHY_1M,
        .secondary_phy = BLE_HCI_LE_PHY_1M,
        .tx_power = tx_power,
        .sid = 0,
    };

    // Configure must come before setting payloads.
    rc = ble_gap_ext_adv_configure(0,
        &adv_params,
        NULL,
        _advertising_event, self);
    if (rc != NIMBLE_OK) {
        return rc;
    }

    os_mbuf_t buf;
    _wrap_in_mbuf(advertising_data, advertising_data_len, &buf);
    // This copies the advertising data into buffers to send to the controller.
    // Therefore, we don't need to worry about the lifetime of our copy.
    rc = ble_gap_ext_adv_set_data(0, &buf.mbuf);
    if (rc != NIMBLE_OK) {
        return rc;
    }

    if (scan_response_data_len > 0) {
        _wrap_in_mbuf(scan_response_data, scan_response_data_len, &buf);
        // This copies the advertising data into buffers to send to the controller.
        // Therefore, we don't need to worry about the lifetime of our copy.
        rc = ble_gap_ext_adv_rsp_set_data(0, &buf.mbuf);
        if (rc != NIMBLE_OK) {
            return rc;
        }
    }

    rc = ble_gap_ext_adv_start(0, timeout / 10, 0);
    if (rc != NIMBLE_OK) {
        return rc;
    }

    return NIMBLE_OK;
}

STATIC void check_data_fit(size_t data_len, bool connectable) {
    if (data_len > MYNEWT_VAL(BLE_EXT_ADV_MAX_SIZE) ||
        (connectable && data_len > MYNEWT_VAL(BLE_EXT_ADV_MAX_SIZE))) {
        mp_raise_ValueError(translate("Data too large for advertisement packet"));
    }
}

void common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self, bool connectable,
    bool anonymous, uint32_t timeout, mp_float_t interval,
    mp_buffer_info_t *advertising_data_bufinfo, mp_buffer_info_t *scan_response_data_bufinfo,
    mp_int_t tx_power, const bleio_address_obj_t *directed_to) {
    if (self->user_advertising) {
        mp_raise_bleio_BluetoothError(translate("Already advertising."));
    } else {
        // If the user isn't advertising, then the background is. So, stop the
        // background advertising so the user can.
        common_hal_bleio_adapter_stop_advertising(self);
    }
    // interval value has already been validated.

    check_data_fit(advertising_data_bufinfo->len, connectable);
    check_data_fit(scan_response_data_bufinfo->len, connectable);

    if (advertising_data_bufinfo->len > 31 && scan_response_data_bufinfo->len > 0) {
        mp_raise_bleio_BluetoothError(translate("Extended advertisements with scan response not supported."));
    }


    if (advertising_data_bufinfo->len > 0 && directed_to != NULL) {
        mp_raise_bleio_BluetoothError(translate("Data not supported with directed advertising"));
    }

    if (anonymous) {
        mp_raise_NotImplementedError(NULL);
    }

    if (!timeout) {
        timeout = BLE_HS_FOREVER;
    } else if (timeout > INT32_MAX) {
        mp_raise_bleio_BluetoothError(translate("Timeout is too long: Maximum timeout length is %d seconds"),
            INT32_MAX / 1000);
    }

    CHECK_NIMBLE_ERROR(_common_hal_bleio_adapter_start_advertising(self, connectable, anonymous, timeout, interval,
        advertising_data_bufinfo->buf,
        advertising_data_bufinfo->len,
        scan_response_data_bufinfo->buf,
        scan_response_data_bufinfo->len,
        tx_power,
        directed_to));
    self->user_advertising = true;
}

void common_hal_bleio_adapter_stop_advertising(bleio_adapter_obj_t *self) {
    int err_code = ble_gap_ext_adv_stop(0);
    self->user_advertising = false;

    if ((err_code != NIMBLE_OK) &&
        (err_code != BLE_HS_EALREADY) &&
        (err_code != BLE_HS_EINVAL)) {
        CHECK_NIMBLE_ERROR(err_code);
    }
}

bool common_hal_bleio_adapter_get_advertising(bleio_adapter_obj_t *self) {
    return ble_gap_adv_active();
}

bool common_hal_bleio_adapter_get_connected(bleio_adapter_obj_t *self) {
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn_handle != BLEIO_HANDLE_INVALID) {
            return true;
        }
    }
    return false;
}

mp_obj_t common_hal_bleio_adapter_get_connections(bleio_adapter_obj_t *self) {
    if (self->connection_objs != NULL) {
        return self->connection_objs;
    }
    size_t total_connected = 0;
    mp_obj_t items[BLEIO_TOTAL_CONNECTION_COUNT];
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn_handle != BLEIO_HANDLE_INVALID) {
            if (connection->connection_obj == mp_const_none) {
                connection->connection_obj = bleio_connection_new_from_internal(connection);
            }
            items[total_connected] = connection->connection_obj;
            total_connected++;
        }
    }
    self->connection_objs = mp_obj_new_tuple(total_connected, items);
    return self->connection_objs;
}

void common_hal_bleio_adapter_erase_bonding(bleio_adapter_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
    // bonding_erase_storage();
}

bool common_hal_bleio_adapter_is_bonded_to_central(bleio_adapter_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
    // return bonding_peripheral_bond_count() > 0;
    return false;
}

void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter) {
    // We divide by size_t so that we can scan each 32-bit aligned value to see
    // if it is a pointer. This allows us to change the structs without worrying
    // about collecting new pointers.
    gc_collect_root((void **)adapter, sizeof(bleio_adapter_obj_t) / (sizeof(size_t)));
    gc_collect_root((void **)bleio_connections, sizeof(bleio_connections) / (sizeof(size_t)));
}

void bleio_adapter_reset(bleio_adapter_obj_t *adapter) {
    common_hal_bleio_adapter_stop_scan(adapter);
    if (common_hal_bleio_adapter_get_advertising(adapter)) {
        common_hal_bleio_adapter_stop_advertising(adapter);
    }

    adapter->connection_objs = NULL;
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        // Disconnect all connections cleanly.
        if (connection->conn_handle != BLEIO_HANDLE_INVALID) {
            common_hal_bleio_connection_disconnect(connection);
        }
        connection->connection_obj = mp_const_none;
    }

    // Wait up to 125 ms (128 ticks) for disconnect to complete. This should be
    // greater than most connection intervals.
    bool any_connected = false;
    uint64_t start_ticks = supervisor_ticks_ms64();
    while (any_connected && supervisor_ticks_ms64() - start_ticks < 128) {
        any_connected = false;
        for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
            bleio_connection_internal_t *connection = &bleio_connections[i];
            any_connected |= connection->conn_handle != BLEIO_HANDLE_INVALID;
        }
    }
}
