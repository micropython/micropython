// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

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
#include "services/ans/ble_svc_ans.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "bindings/espidf/__init__.h"
#include "common-hal/_bleio/Connection.h"

#include "esp_bt.h"
#include "esp_mac.h"
#include "esp_nimble_hci.h"
#include "nvs_flash.h"

#if CIRCUITPY_OS_GETENV
#include "shared-module/os/__init__.h"
#endif

bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

bool ble_active = false;

static void nimble_host_task(void *param) {
    nimble_port_run();
    nimble_port_freertos_deinit();
}

static TaskHandle_t cp_task = NULL;

static void _on_sync(void) {
    int rc = ble_hs_util_ensure_addr(false);
    assert(rc == 0);

    xTaskNotifyGive(cp_task);
}

// All examples have this. It'd make sense in a header.
void ble_store_config_init(void);

char default_ble_name[] = { 'C', 'I', 'R', 'C', 'U', 'I', 'T', 'P', 'Y', 0, 0, 0, 0, 0, 0, 0};

void common_hal_bleio_adapter_set_enabled(bleio_adapter_obj_t *self, bool enabled) {
    const bool is_enabled = common_hal_bleio_adapter_get_enabled(self);

    // Don't enable or disable twice
    if (is_enabled == enabled) {
        return;
    }

    if (enabled) {
        CHECK_ESP_RESULT(nimble_port_init());

        // ble_hs_cfg.reset_cb = blecent_on_reset;
        ble_hs_cfg.sync_cb = _on_sync;
        // ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

        ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;
        ble_hs_cfg.sm_bonding = 1;
        /* Enable the appropriate bit masks to make sure the keys
         * that are needed are exchanged
         */
        ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;
        ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC;

        ble_hs_cfg.sm_mitm = 0;
        ble_hs_cfg.sm_sc = 0;
        /* Stores the IRK */
        ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ID;
        ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ID;

        ble_svc_gap_init();
        ble_svc_gatt_init();
        ble_svc_ans_init();

        #if CIRCUITPY_OS_GETENV
        char ble_name[1 + MYNEWT_VAL_BLE_SVC_GAP_DEVICE_NAME_MAX_LENGTH];
        os_getenv_err_t result = common_hal_os_getenv_str("CIRCUITPY_BLE_NAME", ble_name, sizeof(ble_name));
        if (result == GETENV_OK) {
            ble_svc_gap_device_name_set(ble_name);
        } else
        #endif
        {
            uint8_t mac[6];
            esp_read_mac(mac, ESP_MAC_BT);
            mp_int_t len = sizeof(default_ble_name) - 1;
            default_ble_name[len - 6] = nibble_to_hex_lower[mac[3] >> 4 & 0xf];
            default_ble_name[len - 5] = nibble_to_hex_lower[mac[3] & 0xf];
            default_ble_name[len - 4] = nibble_to_hex_lower[mac[4] >> 4 & 0xf];
            default_ble_name[len - 3] = nibble_to_hex_lower[mac[4] & 0xf];
            default_ble_name[len - 2] = nibble_to_hex_lower[mac[5] >> 4 & 0xf];
            default_ble_name[len - 1] = nibble_to_hex_lower[mac[5] & 0xf];
            default_ble_name[len] = '\0'; // for now we add null for compatibility with C ASCIIZ strings
            ble_svc_gap_device_name_set(default_ble_name);
        }

        // Clear all of the internal connection objects.
        for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
            bleio_connection_internal_t *connection = &bleio_connections[i];
            // Reset connection.
            connection->conn_handle = BLEIO_HANDLE_INVALID;
        }

        ble_store_config_init();

        cp_task = xTaskGetCurrentTaskHandle();

        nimble_port_freertos_init(nimble_host_task);
        // Wait for sync.
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(200));
    } else {
        int ret = nimble_port_stop();
        while (xTaskGetHandle("nimble_host") != NULL) {
            vTaskDelay(pdMS_TO_TICKS(2));
        }
        if (ret == 0) {
            nimble_port_deinit();
        }
    }
}

bool common_hal_bleio_adapter_get_enabled(bleio_adapter_obj_t *self) {
    return xTaskGetHandle("nimble_host") != NULL;
}

bleio_address_obj_t *common_hal_bleio_adapter_get_address(bleio_adapter_obj_t *self) {
    uint8_t address_bytes[6];
    uint8_t address_type = BLE_ADDR_RANDOM;
    ble_hs_id_infer_auto(0, &address_type);
    int result = ble_hs_id_copy_addr(address_type, address_bytes, NULL);
    if (result != 0) {
        return NULL;
    }

    bleio_address_obj_t *address = mp_obj_malloc(bleio_address_obj_t, &bleio_address_type);
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

uint16_t bleio_adapter_get_name(char *buf, uint16_t len) {
    const char *name = ble_svc_gap_device_name();
    uint16_t full_len = strlen(name);
    memcpy(buf, name, MIN(full_len, len));

    return full_len;
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
        #if MYNEWT_VAL(BLE_EXT_ADV)
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
        #endif
    }

    return 0;
}

mp_obj_t common_hal_bleio_adapter_start_scan(bleio_adapter_obj_t *self, uint8_t *prefixes,
    size_t prefix_length, bool extended, mp_int_t buffer_size, mp_float_t timeout,
    mp_float_t interval, mp_float_t window, mp_int_t minimum_rssi, bool active) {
    if (self->scan_results != NULL) {
        if (!shared_module_bleio_scanresults_get_done(self->scan_results)) {
            mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Scan already in progress. Stop with stop_scan."));
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

static void _convert_address(const bleio_address_obj_t *address, ble_addr_t *nimble_address) {
    nimble_address->type = address->type;
    mp_buffer_info_t address_buf_info;
    mp_get_buffer_raise(address->bytes, &address_buf_info, MP_BUFFER_READ);
    memcpy(nimble_address->val, (uint8_t *)address_buf_info.buf, NUM_BLEIO_ADDRESS_BYTES);
}

static int _mtu_reply(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    uint16_t mtu, void *arg) {
    bleio_connection_internal_t *connection = (bleio_connection_internal_t *)arg;
    if (conn_handle != connection->conn_handle) {
        return 0;
    }
    if (error->status == 0) {
        connection->mtu = mtu;
    }
    xTaskNotify(cp_task, conn_handle, eSetValueWithOverwrite);
    return 0;
}

static void _new_connection(uint16_t conn_handle) {
    // Set the tx_power for the connection higher than the advertisement.
    esp_ble_tx_power_set(conn_handle, ESP_PWR_LVL_N0);


    // Find an empty connection. One should always be available because the SD has the same
    // total connection limit.
    bleio_connection_internal_t *connection = NULL;
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        connection = &bleio_connections[i];
        if (connection->conn_handle == BLEIO_HANDLE_INVALID) {
            break;
        }
    }

    // Shouldn't happen, but just return if no connection available.
    if (!connection) {
        return;
    }

    connection->conn_handle = conn_handle;
    connection->connection_obj = mp_const_none;
    connection->pair_status = PAIR_NOT_PAIRED;
    connection->mtu = 0;

    ble_gattc_exchange_mtu(conn_handle, _mtu_reply, connection);

    // Change the callback for the connection.
    ble_gap_set_event_cb(conn_handle, bleio_connection_event_cb, connection);
}

static int _connect_event(struct ble_gap_event *event, void *self_in) {
    bleio_adapter_obj_t *self = (bleio_adapter_obj_t *)self_in;

    #if CIRCUITPY_VERBOSE_BLE
    mp_printf(&mp_plat_print, "Connect event: %d\n", event->type);
    #endif
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                // This triggers an MTU exchange. Its reply will unblock CP.
                _new_connection(event->connect.conn_handle);
                // Set connections objs back to NULL since we have a new
                // connection and need a new tuple.
                self->connection_objs = NULL;
            } else {
                xTaskNotify(cp_task, -event->connect.status, eSetValueWithOverwrite);
            }
            break;

        default:
            #if CIRCUITPY_VERBOSE_BLE
            // For debugging.
            mp_printf(&mp_plat_print, "Unhandled connect event: %d\n", event->type);
            #endif
            break;
    }
    return 0;
}

mp_obj_t common_hal_bleio_adapter_connect(bleio_adapter_obj_t *self, bleio_address_obj_t *address, mp_float_t timeout) {
    // Stop any active scan.
    if (self->scan_results != NULL) {
        common_hal_bleio_adapter_stop_scan(self);
    }

    struct ble_gap_conn_params conn_params = {
        .scan_itvl = MSEC_TO_UNITS(100, UNIT_0_625_MS),
        .scan_window = MSEC_TO_UNITS(100, UNIT_0_625_MS),
        .itvl_min = MSEC_TO_UNITS(15, UNIT_1_25_MS),
        .itvl_max = MSEC_TO_UNITS(300, UNIT_1_25_MS),
        .latency = 0,
        .supervision_timeout = MSEC_TO_UNITS(4000, UNIT_10_MS),
        .min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN,
        .max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN
    };

    uint8_t own_addr_type;
    // TODO: Use a resolvable address if the peer has our key.
    CHECK_NIMBLE_ERROR(ble_hs_id_infer_auto(false, &own_addr_type));

    ble_addr_t addr;
    _convert_address(address, &addr);

    cp_task = xTaskGetCurrentTaskHandle();
    // Make sure we don't have a pending notification from a previous time. This
    // can happen if a previous wait timed out before the notification was given.
    xTaskNotifyStateClear(cp_task);
    CHECK_NIMBLE_ERROR(
        ble_gap_connect(own_addr_type, &addr,
            SEC_TO_UNITS(timeout, UNIT_1_MS) + 0.5f,
            &conn_params,
            _connect_event, self));

    int error_code;
    // Wait an extra 50 ms to give the connect method the opportunity to time out.
    CHECK_NOTIFY(xTaskNotifyWait(0, 0, (uint32_t *)&error_code, pdMS_TO_TICKS(timeout * 1000 + 50)));
    // Negative values are error codes, connection handle otherwise.
    if (error_code < 0) {
        CHECK_BLE_ERROR(-error_code);
    }
    uint16_t conn_handle = error_code;

    // TODO: If we have keys, then try and encrypt the connection.

    // TODO: Negotiate for better PHY and data lengths since we are the central. These are
    // nice-to-haves so ignore any errors.

    // Make the connection object and return it.
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn_handle == conn_handle) {
            connection->is_central = true;
            return bleio_connection_new_from_internal(connection);
        }
    }

    mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Failed to connect: internal error"));

    return mp_const_none;
}

#if MYNEWT_VAL(BLE_EXT_ADV)
typedef struct {
    struct os_mbuf mbuf;
    struct os_mbuf_pkthdr hdr;
} os_mbuf_t;

static void _wrap_in_mbuf(const uint8_t *data, uint16_t len, os_mbuf_t *buf) {
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
#endif

static int _advertising_event(struct ble_gap_event *event, void *self_in) {
    bleio_adapter_obj_t *self = (bleio_adapter_obj_t *)self_in;
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            // Spurious connect events can happen.

            #if !MYNEWT_VAL(BLE_EXT_ADV)
            if (event->connect.status == NIMBLE_OK) {
                _new_connection(event->connect.conn_handle);
                // Set connections objs back to NULL since we have a new
                // connection and need a new tuple.
                self->connection_objs = NULL;
            }
            common_hal_bleio_adapter_stop_advertising(self);
            #endif

            break;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            #if MYNEWT_VAL(BLE_EXT_ADV)
            if (event->adv_complete.reason == NIMBLE_OK) {
                _new_connection(event->adv_complete.conn_handle);
                // Set connections objs back to NULL since we have a new
                // connection and need a new tuple.
                self->connection_objs = NULL;
            }
            #endif
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
    background_callback_add_core(&bleio_background_callback);
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
    // Override anonymous because it isn't working with the ESP-IDF.
    anonymous = false;

    uint32_t rc;

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

    #if MYNEWT_VAL(BLE_EXT_ADV)
    bool extended = advertising_data_len > BLE_ADV_LEGACY_DATA_MAX_LEN ||
        scan_response_data_len > BLE_ADV_LEGACY_DATA_MAX_LEN;

    bool scannable = scan_response_data_len > 0;
    bool legacy_pdu = !extended && !anonymous;
    if (legacy_pdu && connectable) {
        // Connectable legacy advertisements are always scannable too.
        scannable = true;
    }

    struct ble_gap_ext_adv_params adv_params = {
        .connectable = connectable,
        .scannable = scannable,
        .directed = directed_to != NULL,
        .high_duty_directed = high_duty_directed,
        .legacy_pdu = legacy_pdu,
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
    #else
    uint8_t conn_mode = connectable ? BLE_GAP_CONN_MODE_UND : BLE_GAP_CONN_MODE_NON;
    if (directed_to != NULL) {
        conn_mode = BLE_GAP_CONN_MODE_DIR;
    }

    struct ble_gap_adv_params adv_params = {
        .conn_mode = conn_mode,
        .disc_mode = BLE_GAP_DISC_MODE_GEN,
        .itvl_min = SEC_TO_UNITS(interval, UNIT_0_625_MS) + 0.5f,
        .itvl_max = SEC_TO_UNITS(interval, UNIT_0_625_MS) + 0.5f,
        .channel_map = 0,
        .filter_policy = BLE_HCI_CONN_FILT_NO_WL,
        .high_duty_cycle = high_duty_directed,
    };

    rc = ble_gap_adv_set_data(advertising_data, advertising_data_len);
    if (rc != NIMBLE_OK) {
        return rc;
    }

    if (scan_response_data_len > 0) {
        rc = ble_gap_adv_rsp_set_data(scan_response_data, scan_response_data_len);
        if (rc != NIMBLE_OK) {
            return rc;
        }
    }
    rc = ble_gap_adv_start(own_addr_type, directed_to != NULL ? &peer: NULL,
        timeout / 10,
        &adv_params,
        _advertising_event, self);
    #endif

    return rc;
}

static void check_data_fit(size_t data_len, bool connectable) {
    if (data_len > MYNEWT_VAL(BLE_EXT_ADV_MAX_SIZE) ||
        (connectable && data_len > MYNEWT_VAL(BLE_EXT_ADV_MAX_SIZE))) {
        mp_raise_ValueError(MP_ERROR_TEXT("Data too large for advertisement packet"));
    }
}

void common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self, bool connectable,
    bool anonymous, uint32_t timeout, mp_float_t interval,
    mp_buffer_info_t *advertising_data_bufinfo, mp_buffer_info_t *scan_response_data_bufinfo,
    mp_int_t tx_power, const bleio_address_obj_t *directed_to) {
    if (self->user_advertising) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Already advertising."));
    } else {
        // If the user isn't advertising, then the background is. So, stop the
        // background advertising so the user can.
        common_hal_bleio_adapter_stop_advertising(self);
    }
    // interval value has already been validated.

    check_data_fit(advertising_data_bufinfo->len, connectable);
    check_data_fit(scan_response_data_bufinfo->len, connectable);

    if (advertising_data_bufinfo->len > 31 && scan_response_data_bufinfo->len > 0) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Extended advertisements with scan response not supported."));
    }


    if (advertising_data_bufinfo->len > 0 && directed_to != NULL) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Data not supported with directed advertising"));
    }

    if (anonymous) {
        mp_raise_NotImplementedError(NULL);
    }

    if (!timeout) {
        timeout = BLE_HS_FOREVER;
    } else if (timeout > INT32_MAX) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Timeout is too long: Maximum timeout length is %d seconds"),
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
    self->user_advertising = false;
    if (!common_hal_bleio_adapter_get_advertising(self)) {
        return;
    }
    #if MYNEWT_VAL(BLE_EXT_ADV)
    int err_code = ble_gap_ext_adv_stop(0);
    #else
    int err_code = ble_gap_adv_stop();
    #endif

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
        if (connection->conn_handle != BLEIO_HANDLE_INVALID && connection->mtu != 0) {
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
        if (connection->conn_handle != BLEIO_HANDLE_INVALID && connection->mtu != 0) {
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

#define NIMBLE_NVS_PEER_SEC_KEY                  "peer_sec"
#define NIMBLE_NVS_OUR_SEC_KEY                   "our_sec"
#define NIMBLE_NVS_CCCD_SEC_KEY                  "cccd_sec"
#define NIMBLE_NVS_PEER_RECORDS_KEY              "p_dev_rec"
#define NIMBLE_NVS_NAMESPACE                     "nimble_bond"

// Implement bonding control ourselves when the adapter isn't enabled so that it
// can run when BLE is off.
void common_hal_bleio_adapter_erase_bonding(bleio_adapter_obj_t *self) {
    if (common_hal_bleio_adapter_get_enabled(self)) {
        ble_store_clear();
    } else {
        nvs_handle_t nimble_handle;
        esp_err_t err = nvs_open(NIMBLE_NVS_NAMESPACE, NVS_READWRITE, &nimble_handle);
        if (err != ESP_OK) {
            return;
        }
        nvs_erase_all(nimble_handle);
        nvs_commit(nimble_handle);
        nvs_close(nimble_handle);
    }
}

bool common_hal_bleio_adapter_is_bonded_to_central(bleio_adapter_obj_t *self) {
    if (common_hal_bleio_adapter_get_enabled(self)) {
        int count;
        ble_store_util_count(BLE_STORE_OBJ_TYPE_PEER_SEC, &count);
        return count > 0;
    }
    nvs_handle_t nimble_handle;
    esp_err_t err = nvs_open(NIMBLE_NVS_NAMESPACE, NVS_READONLY, &nimble_handle);
    if (err != ESP_OK) {
        return false;
    }
    err = nvs_find_key(nimble_handle, "peer_sec_1", NULL);
    nvs_close(nimble_handle);
    if (err == ESP_OK) {
        return true;
    }
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
