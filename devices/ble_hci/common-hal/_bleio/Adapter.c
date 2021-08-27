/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
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

#include "hci.h"

#include "py/gc.h"
#include "py/mphal.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/tick.h"
#include "supervisor/usb.h"
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/ScanEntry.h"
#include "shared-bindings/time/__init__.h"

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))
#define SEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000000) / (RESOLUTION))
#define UNITS_TO_SEC(TIME, RESOLUTION) (((TIME)*(RESOLUTION)) / 1000000)
// 0.625 msecs (625 usecs)
#define ADV_INTERVAL_UNIT_FLOAT_SECS (0.000625)
// Microseconds is the base unit. The macros above know that.
#define UNIT_0_625_MS (625)
#define UNIT_1_25_MS  (1250)
#define UNIT_10_MS    (10000)

#define MAX_ADVERTISEMENT_SIZE (31)

// TODO make this settable from Python.
#define DEFAULT_TX_POWER 0  // 0 dBm
#define MAX_ANONYMOUS_ADV_TIMEOUT_SECS (60 * 15)
#define MAX_LIMITED_DISCOVERABLE_ADV_TIMEOUT_SECS (180)

#define BLE_MIN_CONN_INTERVAL        MSEC_TO_UNITS(15, UNIT_0_625_MS)
#define BLE_MAX_CONN_INTERVAL        MSEC_TO_UNITS(15, UNIT_0_625_MS)
#define BLE_SLAVE_LATENCY            0
#define BLE_CONN_SUP_TIMEOUT         MSEC_TO_UNITS(4000, UNIT_10_MS)

bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

STATIC void add_generic_services(bleio_adapter_obj_t *adapter) {
    // Create Generic Access UUID, Service, and Characteristics.

    // Generic Access Service setup.

    bleio_uuid_obj_t *generic_access_service_uuid = m_new_obj(bleio_uuid_obj_t);
    generic_access_service_uuid->base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(generic_access_service_uuid, 0x1800, NULL);

    bleio_uuid_obj_t *device_name_characteristic_uuid = m_new_obj(bleio_uuid_obj_t);
    device_name_characteristic_uuid->base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(device_name_characteristic_uuid, 0x2A00, NULL);

    bleio_uuid_obj_t *appearance_characteristic_uuid = m_new_obj(bleio_uuid_obj_t);
    appearance_characteristic_uuid->base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(appearance_characteristic_uuid, 0x2A01, NULL);

    // Not implemented:
    // Peripheral Preferred Connection Parameters
    // Central Address Resolution

    bleio_service_obj_t *generic_access_service = m_new_obj(bleio_service_obj_t);
    generic_access_service->base.type = &bleio_service_type;
    common_hal_bleio_service_construct(generic_access_service, generic_access_service_uuid, false);

    adapter->device_name_characteristic = m_new_obj(bleio_characteristic_obj_t);
    adapter->device_name_characteristic->base.type = &bleio_characteristic_type;

    char generic_name[] = { 'C', 'I', 'R', 'C', 'U', 'I', 'T', 'P', 'Y', 'n', 'n', 'n', 'n' };
    mp_buffer_info_t generic_name_bufinfo = {
        .buf = generic_name,
        .len = sizeof(generic_name),
    };

    // Will be added to service by constructor.
    common_hal_bleio_characteristic_construct(
        adapter->device_name_characteristic,
        generic_access_service,
        BLE_GATT_HANDLE_INVALID,
        device_name_characteristic_uuid,
        CHAR_PROP_READ,
        SECURITY_MODE_OPEN,
        SECURITY_MODE_NO_ACCESS,
        248,                    // max length, from Bluetooth spec
        false,                  // not fixed length
        &generic_name_bufinfo,
        NULL
        );

    uint16_t zero_16 = 0;
    mp_buffer_info_t zero_16_value = {
        .buf = &zero_16,
        .len = sizeof(zero_16),
    };

    adapter->appearance_characteristic = m_new_obj(bleio_characteristic_obj_t);
    adapter->appearance_characteristic->base.type = &bleio_characteristic_type;

    common_hal_bleio_characteristic_construct(
        adapter->appearance_characteristic,
        generic_access_service,
        BLE_GATT_HANDLE_INVALID,
        appearance_characteristic_uuid,
        CHAR_PROP_READ,
        SECURITY_MODE_OPEN,
        SECURITY_MODE_NO_ACCESS,
        2,                      // max length, from Bluetooth spec
        true,                  // fixed length
        &zero_16_value,
        NULL
        );

    // Generic Attribute Service setup.

    bleio_uuid_obj_t *generic_attribute_service_uuid = m_new_obj(bleio_uuid_obj_t);
    generic_attribute_service_uuid->base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(generic_attribute_service_uuid, 0x1801, NULL);

    bleio_uuid_obj_t *service_changed_characteristic_uuid = m_new_obj(bleio_uuid_obj_t);
    service_changed_characteristic_uuid->base.type = &bleio_uuid_type;
    common_hal_bleio_uuid_construct(service_changed_characteristic_uuid, 0x2A05, NULL);

    bleio_service_obj_t *generic_attribute_service = m_new_obj(bleio_service_obj_t);
    generic_attribute_service->base.type = &bleio_service_type;
    common_hal_bleio_service_construct(generic_attribute_service, generic_attribute_service_uuid, false);

    adapter->service_changed_characteristic = m_new_obj(bleio_characteristic_obj_t);
    adapter->service_changed_characteristic->base.type = &bleio_characteristic_type;

    uint32_t zero_32 = 0;
    mp_buffer_info_t zero_32_value = {
        .buf = &zero_32,
        .len = sizeof(zero_32),
    };

    common_hal_bleio_characteristic_construct(
        adapter->service_changed_characteristic,
        generic_attribute_service,
        BLE_GATT_HANDLE_INVALID,
        service_changed_characteristic_uuid,
        CHAR_PROP_INDICATE,
        SECURITY_MODE_OPEN,
        SECURITY_MODE_NO_ACCESS,
        4,                     // max length, from Bluetooth spec
        true,                  // fixed length
        &zero_32_value,
        NULL
        );
}


STATIC void check_enabled(bleio_adapter_obj_t *adapter) {
    if (!common_hal_bleio_adapter_get_enabled(adapter)) {
        mp_raise_bleio_BluetoothError(translate("Adapter not enabled"));
    }
}

// STATIC bool adapter_on_ble_evt(ble_evt_t *ble_evt, void *self_in) {
//     bleio_adapter_obj_t *self = (bleio_adapter_obj_t*)self_in;

//     // For debugging.
//     // mp_printf(&mp_plat_print, "Adapter event: 0x%04x\n", ble_evt->header.evt_id);

//     switch (ble_evt->header.evt_id) {
//         case BLE_GAP_EVT_CONNECTED: {
//             // Find an empty connection. One must always be available because the SD has the same
//             // total connection limit.
//             bleio_connection_internal_t *connection;
//             for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
//                 connection = &bleio_connections[i];
//                 if (connection->conn_handle == BLE_CONN_HANDLE_INVALID) {
//                     break;
//                 }
//             }

//             // Central has connected.
//             ble_gap_evt_connected_t* connected = &ble_evt->evt.gap_evt.params.connected;

//             connection->conn_handle = ble_evt->evt.gap_evt.conn_handle;
//             connection->connection_obj = mp_const_none;
//             connection->pair_status = PAIR_NOT_PAIRED;
//             connection->mtu = 0;

//             ble_drv_add_event_handler_entry(&connection->handler_entry, connection_on_ble_evt, connection);
//             self->connection_objs = NULL;

//             // Save the current connection parameters.
//             memcpy(&connection->conn_params, &connected->conn_params, sizeof(ble_gap_conn_params_t));

//             #if CIRCUITPY_VERBOSE_BLE
//             ble_gap_conn_params_t *cp = &connected->conn_params;
//             mp_printf(&mp_plat_print, "conn params: min_ci %d max_ci %d s_l %d sup_timeout %d\n", cp->min_conn_interval, cp->max_conn_interval, cp->slave_latency, cp->conn_sup_timeout);
//             #endif

//             // See if connection interval set by Central is out of range.
//             // If so, negotiate our preferred range.
//             ble_gap_conn_params_t conn_params;
//             sd_ble_gap_ppcp_get(&conn_params);
//             if (conn_params.min_conn_interval < connected->conn_params.min_conn_interval ||
//                 conn_params.min_conn_interval > connected->conn_params.max_conn_interval) {
//                 sd_ble_gap_conn_param_update(ble_evt->evt.gap_evt.conn_handle, &conn_params);
//             }
//             self->current_advertising_data = NULL;
//             break;
//         }
//         case BLE_GAP_EVT_DISCONNECTED: {
//             // Find the connection that was disconnected.
//             bleio_connection_internal_t *connection;
//             for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
//                 connection = &bleio_connections[i];
//                 if (connection->conn_handle == ble_evt->evt.gap_evt.conn_handle) {
//                     break;
//                 }
//             }
//             ble_drv_remove_event_handler(connection_on_ble_evt, connection);
//             connection->conn_handle = BLE_CONN_HANDLE_INVALID;
//             connection->pair_status = PAIR_NOT_PAIRED;
//             if (connection->connection_obj != mp_const_none) {
//                 bleio_connection_obj_t* obj = connection->connection_obj;
//                 obj->connection = NULL;
//                 obj->disconnect_reason = ble_evt->evt.gap_evt.params.disconnected.reason;
//             }
//             self->connection_objs = NULL;

//             break;
//         }

//         case BLE_GAP_EVT_ADV_SET_TERMINATED:
//             self->current_advertising_data = NULL;
//             break;

//         default:
//             // For debugging.
//             // mp_printf(&mp_plat_print, "Unhandled adapter event: 0x%04x\n", ble_evt->header.evt_id);
//             return false;
//             break;
//     }
//     return true;
// }

char default_ble_name[] = { 'C', 'I', 'R', 'C', 'U', 'I', 'T', 'P', 'Y', 0, 0, 0, 0};

// Get various values and limits set by the adapter.
// Set event mask.
STATIC void bleio_adapter_hci_init(bleio_adapter_obj_t *self) {

    const size_t len = sizeof(default_ble_name);

    bt_addr_t addr;
    hci_check_error(hci_read_bd_addr(&addr));

    default_ble_name[len - 4] = nibble_to_hex_lower[addr.val[1] >> 4 & 0xf];
    default_ble_name[len - 3] = nibble_to_hex_lower[addr.val[1] & 0xf];
    default_ble_name[len - 2] = nibble_to_hex_lower[addr.val[0] >> 4 & 0xf];
    default_ble_name[len - 1] = nibble_to_hex_lower[addr.val[0] & 0xf];
    self->name = mp_obj_new_str(default_ble_name, len);

    // Get version information.
    if (hci_read_local_version(&self->hci_version, &self->hci_revision, &self->lmp_version,
        &self->manufacturer, &self->lmp_subversion) != HCI_OK) {
        mp_raise_bleio_BluetoothError(translate("Could not read HCI version"));
    }
    // Get supported features.
    if (hci_le_read_local_supported_features(self->features) != HCI_OK) {
        mp_raise_bleio_BluetoothError(translate("Could not read BLE features"));
    }

    // Enabled desired events.
    // Most importantly, includes:
    // BT_EVT_MASK_LE_META_EVENT                BT_EVT_BIT(61)
    if (hci_set_event_mask(0x3FFFFFFFFFFFFFFF) != HCI_OK) {
        mp_raise_bleio_BluetoothError(translate("Could not set event mask"));
    }
    // The default events for LE are:
    // BT_EVT_MASK_LE_CONN_COMPLETE,  BT_EVT_MASK_LE_ADVERTISING_REPORT,
    // BT_EVT_MASK_LE_CONN_UPDATE_COMPLETE, BT_EVT_MASK_LE_REMOTE_FEAT_COMPLETE
    // BT_EVT_MASK_LE_LTK_REQUEST.
    // That's all we need right now, so we don't bother to set the LE event mask.

    // Get ACL buffer info.
    uint16_t le_max_len;
    uint8_t le_max_num;
    if (hci_le_read_buffer_size(&le_max_len, &le_max_num) == HCI_OK) {
        self->max_acl_buffer_len = le_max_len;
        self->max_acl_num_buffers = le_max_num;
    } else {
        // LE Read Buffer Size not available; use the general Read Buffer Size.
        uint16_t acl_max_len;
        uint8_t sco_max_len;
        uint16_t acl_max_num;
        uint16_t sco_max_num;
        if (hci_read_buffer_size(&acl_max_len, &sco_max_len, &acl_max_num, &sco_max_num) != HCI_OK) {
            mp_raise_bleio_BluetoothError(translate("Could not read BLE buffer info"));
        }
        self->max_acl_buffer_len = acl_max_len;
        self->max_acl_num_buffers = acl_max_num;
    }

    // Get max advertising length if extended advertising is supported.
    if (BT_FEAT_LE_EXT_ADV(self->features)) {
        uint16_t max_adv_data_len;
        if (hci_le_read_maximum_advertising_data_length(&max_adv_data_len) != HCI_OK) {
            mp_raise_bleio_BluetoothError(translate("Could not get max advertising length"));
        }
        self->max_adv_data_len = max_adv_data_len;
    } else {
        self->max_adv_data_len = MAX_ADVERTISEMENT_SIZE;
    }
}

void common_hal_bleio_adapter_construct_hci_uart(bleio_adapter_obj_t *self, busio_uart_obj_t *uart, digitalio_digitalinout_obj_t *rts, digitalio_digitalinout_obj_t *cts) {
    self->allocated = true;
    self->hci_uart = uart;
    self->rts_digitalinout = rts;
    self->cts_digitalinout = cts;

    // Advertising-related fields are initialized by common_hal_bleio_adapter_set_enabled().
    self->enabled = false;

    common_hal_bleio_adapter_set_enabled(self, true);
    bleio_adapter_hci_init(self);
    common_hal_bleio_adapter_set_name(self, default_ble_name);
}

void common_hal_bleio_adapter_set_enabled(bleio_adapter_obj_t *self, bool enabled) {
    const bool is_enabled = common_hal_bleio_adapter_get_enabled(self);

    // Don't enable or disable twice
    if (is_enabled == enabled) {
        return;
    }

    self->enabled = enabled;

    // We must poll for input from the HCI adapter.
    // TODO Can we instead trigger an interrupt on UART input traffic?
    if (enabled) {
        supervisor_enable_tick();
    } else {
        supervisor_disable_tick();
    }

    // Enabling or disabling: stop any current activity; reset to known state.
    hci_reset();
    self->now_advertising = false;
    self->extended_advertising = false;
    self->circuitpython_advertising = false;
    self->advertising_timeout_msecs = 0;

    if (enabled) {
        // Reset list of known attributes.
        // Indices into the list are handles. Handle 0x0000 designates an invalid handle,
        // so store None there to skip it.
        self->attributes = mp_obj_new_list(0, NULL);
        bleio_adapter_add_attribute(self, mp_const_none);
        add_generic_services(self);
    }
}

bool common_hal_bleio_adapter_get_enabled(bleio_adapter_obj_t *self) {
    return self->enabled;
}

bleio_address_obj_t *common_hal_bleio_adapter_get_address(bleio_adapter_obj_t *self) {
    check_enabled(self);

    bt_addr_t addr;
    hci_check_error(hci_read_bd_addr(&addr));

    bleio_address_obj_t *address = m_new_obj(bleio_address_obj_t);
    address->base.type = &bleio_address_type;

    common_hal_bleio_address_construct(address, addr.val, BT_ADDR_LE_PUBLIC);
    return address;
}

bool common_hal_bleio_adapter_set_address(bleio_adapter_obj_t *self, bleio_address_obj_t *address) {
    mp_buffer_info_t bufinfo;
    if (!mp_get_buffer(address->bytes, &bufinfo, MP_BUFFER_READ)) {
        return false;
    }
    return hci_le_set_random_address(bufinfo.buf) == HCI_OK;
}

mp_obj_str_t *common_hal_bleio_adapter_get_name(bleio_adapter_obj_t *self) {
    return self->name;
}

void common_hal_bleio_adapter_set_name(bleio_adapter_obj_t *self, const char *name) {
    self->name = mp_obj_new_str(name, strlen(name));
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(self->name, &bufinfo, MP_BUFFER_READ);
    bleio_characteristic_set_local_value(self->device_name_characteristic, &bufinfo);
}


// STATIC bool scan_on_ble_evt(ble_evt_t *ble_evt, void *scan_results_in) {
//     bleio_scanresults_obj_t *scan_results = (bleio_scanresults_obj_t*)scan_results_in;

//     if (ble_evt->header.evt_id == BLE_GAP_EVT_TIMEOUT &&
//         ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_SCAN) {
//         shared_module_bleio_scanresults_set_done(scan_results, true);
//         ble_drv_remove_event_handler(scan_on_ble_evt, scan_results);
//         return true;
//     }

//     if (ble_evt->header.evt_id != BLE_GAP_EVT_ADV_REPORT) {
//         return false;
//     }
//     ble_gap_evt_adv_report_t *report = &ble_evt->evt.gap_evt.params.adv_report;

//     shared_module_bleio_scanresults_append(scan_results,
//                                            supervisor_ticks_ms64(),
//                                            report->type.connectable,
//                                            report->type.scan_response,
//                                            report->rssi,
//                                            report->peer_addr.addr,
//                                            report->peer_addr.addr_type,
//                                            report->data.p_data,
//                                            report->data.len);

//     const uint32_t err_code = sd_ble_gap_scan_start(NULL, scan_results->common_hal_data);
//     if (err_code != NRF_SUCCESS) {
//         // TODO: Pass the error into the scan results so it can throw an exception.
//         scan_results->done = true;
//     }
//     return true;
// }

mp_obj_t common_hal_bleio_adapter_start_scan(bleio_adapter_obj_t *self, uint8_t *prefixes, size_t prefix_length, bool extended, mp_int_t buffer_size, mp_float_t timeout, mp_float_t interval, mp_float_t window, mp_int_t minimum_rssi, bool active) {
    // TODO
    mp_raise_NotImplementedError(NULL);
    check_enabled(self);

    if (self->scan_results != NULL) {
        if (!shared_module_bleio_scanresults_get_done(self->scan_results)) {
            mp_raise_bleio_BluetoothError(translate("Scan already in progess. Stop with stop_scan."));
        }
        self->scan_results = NULL;
    }
    self->scan_results = shared_module_bleio_new_scanresults(buffer_size, prefixes, prefix_length, minimum_rssi);

    // size_t max_packet_size = extended ? BLE_GAP_SCAN_BUFFER_EXTENDED_MAX_SUPPORTED : BLE_GAP_SCAN_BUFFER_MAX;
    // uint8_t *raw_data = m_malloc(sizeof(ble_data_t) + max_packet_size, false);
    // ble_data_t * sd_data = (ble_data_t *) raw_data;
    // self->scan_results->common_hal_data = sd_data;
    // sd_data->len = max_packet_size;
    // sd_data->p_data = raw_data + sizeof(ble_data_t);

    // ble_drv_add_event_handler(scan_on_ble_evt, self->scan_results);

    // uint32_t nrf_timeout = SEC_TO_UNITS(timeout, UNIT_10_MS);
    // if (timeout <= 0.0001) {
    //     nrf_timeout = BLE_GAP_SCAN_TIMEOUT_UNLIMITED;
    // }

    // ble_gap_scan_params_t scan_params = {
    //     .extended = extended,
    //     .interval = SEC_TO_UNITS(interval, UNIT_0_625_MS),
    //     .timeout = nrf_timeout,
    //     .window = SEC_TO_UNITS(window, UNIT_0_625_MS),
    //     .scan_phys = BLE_GAP_PHY_1MBPS,
    //     .active = active
    // };
    // uint32_t err_code;
    // vm_used_ble = true;
    // err_code = sd_ble_gap_scan_start(&scan_params, sd_data);

    // if (err_code != NRF_SUCCESS) {
    //     self->scan_results = NULL;
    //     ble_drv_remove_event_handler(scan_on_ble_evt, self->scan_results);
    //     check_nrf_error(err_code);
    // }

    return MP_OBJ_FROM_PTR(self->scan_results);
}

void common_hal_bleio_adapter_stop_scan(bleio_adapter_obj_t *self) {
    // TODO
    mp_raise_NotImplementedError(NULL);
    check_enabled(self);

    // If not already scanning, no problem.
    if (hci_le_set_scan_enable(BT_HCI_LE_SCAN_DISABLE, BT_HCI_LE_SCAN_FILTER_DUP_DISABLE) == HCI_OK) {
        shared_module_bleio_scanresults_set_done(self->scan_results, true);
        self->scan_results = NULL;
    }
}

// typedef struct {
//     uint16_t conn_handle;
//     volatile bool done;
// } connect_info_t;

// STATIC bool connect_on_ble_evt(ble_evt_t *ble_evt, void *info_in) {
//     connect_info_t *info = (connect_info_t*)info_in;

//     switch (ble_evt->header.evt_id) {
//         case BLE_GAP_EVT_CONNECTED:
//             info->conn_handle = ble_evt->evt.gap_evt.conn_handle;
//             info->done = true;

//             break;

//         case BLE_GAP_EVT_TIMEOUT:
//             // Handle will be invalid.
//             info->done = true;
//             break;
//         default:
//             // For debugging.
//             // mp_printf(&mp_plat_print, "Unhandled central event: 0x%04x\n", ble_evt->header.evt_id);
//             return false;
//             break;
//     }
//     return true;
// }

mp_obj_t common_hal_bleio_adapter_connect(bleio_adapter_obj_t *self, bleio_address_obj_t *address, mp_float_t timeout) {
    // TODO
    mp_raise_NotImplementedError(NULL);

    check_enabled(self);

    // ble_gap_addr_t addr;

    // addr.addr_type = address->type;
    // mp_buffer_info_t address_buf_info;
    // mp_get_buffer_raise(address->bytes, &address_buf_info, MP_BUFFER_READ);
    // memcpy(addr.addr, (uint8_t *) address_buf_info.buf, NUM_BLEIO_ADDRESS_BYTES);

    // ble_gap_scan_params_t scan_params = {
    //     .interval = MSEC_TO_UNITS(100, UNIT_0_625_MS),
    //     .window = MSEC_TO_UNITS(100, UNIT_0_625_MS),
    //     .scan_phys = BLE_GAP_PHY_1MBPS,
    //     // timeout of 0 means no timeout
    //     .timeout = SEC_TO_UNITS(timeout, UNIT_10_MS),
    // };

    // ble_gap_conn_params_t conn_params = {
    //     .conn_sup_timeout = MSEC_TO_UNITS(4000, UNIT_10_MS),
    //     .min_conn_interval = MSEC_TO_UNITS(15, UNIT_1_25_MS),
    //     .max_conn_interval = MSEC_TO_UNITS(300, UNIT_1_25_MS),
    //     .slave_latency = 0,          // number of conn events
    // };

    // connect_info_t event_info;
    // ble_drv_add_event_handler(connect_on_ble_evt, &event_info);
    // event_info.done = false;

    vm_used_ble = true;
    // uint32_t err_code = sd_ble_gap_connect(&addr, &scan_params, &conn_params, BLE_CONN_CFG_TAG_CUSTOM);

    // if (err_code != NRF_SUCCESS) {
    //     ble_drv_remove_event_handler(connect_on_ble_evt, &event_info);
    //     check_nrf_error(err_code);
    // }

    // while (!event_info.done) {
    //     RUN_BACKGROUND_TASKS;
    // }

    // ble_drv_remove_event_handler(connect_on_ble_evt, &event_info);

    // uint16_t conn_handle = event_info.conn_handle;
    // if (conn_handle == BLE_CONN_HANDLE_INVALID) {
    //     mp_raise_bleio_BluetoothError(translate("Failed to connect: timeout"));
    // }

    // // Negotiate for better PHY, larger MTU and data lengths since we are the central. These are
    // // nice-to-haves so ignore any errors.
    // ble_gap_phys_t const phys = {
    //     .rx_phys = BLE_GAP_PHY_AUTO,
    //     .tx_phys = BLE_GAP_PHY_AUTO,
    // };
    // sd_ble_gap_phy_update(conn_handle, &phys);
    // sd_ble_gattc_exchange_mtu_request(conn_handle, BLE_GATTS_VAR_ATTR_LEN_MAX);
    // sd_ble_gap_data_length_update(conn_handle, NULL, NULL);

    // Make the connection object and return it.
    // for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
    //     bleio_connection_internal_t *connection = &bleio_connections[i];
    //     if (connection->conn_handle == conn_handle) {
    //         return bleio_connection_new_from_internal(connection);
    //     }
    // }

    mp_raise_bleio_BluetoothError(translate("Failed to connect: internal error"));

    return mp_const_none;
}

STATIC void check_data_fit(size_t data_len, bool connectable) {
    if (data_len > MAX_ADVERTISEMENT_SIZE) {
        mp_raise_ValueError(translate("Data too large for advertisement packet"));
    }
}

// STATIC bool advertising_on_ble_evt(ble_evt_t *ble_evt, void *self_in) {
//     bleio_adapter_obj_t *self = (bleio_adapter_obj_t*)self_in;

//     switch (ble_evt->header.evt_id) {
//         case BLE_GAP_EVT_ADV_SET_TERMINATED:
//             common_hal_bleio_adapter_stop_advertising(self);
//             ble_drv_remove_event_handler(advertising_on_ble_evt, self_in);
//             break;

//         default:
//             // For debugging.
//             // mp_printf(&mp_plat_print, "Unhandled advertising event: 0x%04x\n", ble_evt->header.evt_id);
//             return false;
//             break;
//     }
//     return true;
// }

uint32_t _common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self,
    bool connectable, bool anonymous, uint32_t timeout, float interval,
    const uint8_t *advertising_data, uint16_t advertising_data_len,
    const uint8_t *scan_response_data, uint16_t scan_response_data_len,
    mp_int_t tx_power, const bleio_address_obj_t *directed_to) {
    check_enabled(self);

    if (self->now_advertising) {
        if (self->circuitpython_advertising) {
            common_hal_bleio_adapter_stop_advertising(self);
        } else {
            // User-requested advertising.
            // TODO allow multiple advertisements.
            // Already advertising. Can't advertise twice.
            return 1;
        }
    }

    // Peer address, which we don't use (no directed advertising).
    bt_addr_le_t empty_addr = { 0 };

    bool extended =
        advertising_data_len > self->max_adv_data_len || scan_response_data_len > self->max_adv_data_len;

    if (extended) {
        if (!BT_FEAT_LE_EXT_ADV(self->features)) {
            mp_raise_bleio_BluetoothError(translate("Data length needs extended advertising, but this adapter does not support it"));
        }

        uint16_t props = 0;
        if (connectable) {
            props |= BT_HCI_LE_ADV_PROP_CONN;
        }
        if (scan_response_data_len > 0) {
            props |= BT_HCI_LE_ADV_PROP_SCAN;
        }

        // Advertising interval.
        uint32_t interval_units = SEC_TO_UNITS(interval, UNIT_0_625_MS);

        hci_check_error(
            hci_le_set_extended_advertising_parameters(
                0,              // handle
                props,          // adv properties
                interval_units, // min interval
                interval_units, // max interval
                0b111,          // channel map: channels 37, 38, 39
                anonymous ? BT_ADDR_LE_RANDOM : BT_ADDR_LE_PUBLIC,
                &empty_addr,    // peer_addr,
                0x00,           // filter policy: no filter
                DEFAULT_TX_POWER,
                BT_HCI_LE_EXT_SCAN_PHY_1M, // Secondary PHY to use
                0x00,                      // AUX_ADV_IND shall be sent prior to next adv event
                BT_HCI_LE_EXT_SCAN_PHY_1M, // Secondary PHY to use
                0x00,                      // Advertising SID
                0x00                       // Scan req notify disable
                ));

        // We can use the duration mechanism provided, instead of our own.
        self->advertising_timeout_msecs = 0;

        uint8_t handle[1] = { 0 };
        uint16_t duration_10msec[1] = { timeout * 100 };
        uint8_t max_ext_adv_evts[1] = { 0 };
        hci_check_error(
            hci_le_set_extended_advertising_enable(
                BT_HCI_LE_ADV_ENABLE,
                1,                // one advertising set.
                handle,
                duration_10msec,
                max_ext_adv_evts
                ));

        self->extended_advertising = true;
    } else {
        // Legacy advertising (not extended).

        uint8_t adv_type;
        if (connectable) {
            // Connectable, scannable, undirected.
            adv_type = BT_HCI_ADV_IND;
        } else if (scan_response_data_len > 0) {
            // Unconnectable, scannable, undirected.
            adv_type = BT_HCI_ADV_SCAN_IND;
        } else {
            // Unconnectable, unscannable, undirected.
            adv_type = BT_HCI_ADV_NONCONN_IND;
        }

        // Advertising interval.
        uint16_t interval_units = SEC_TO_UNITS(interval, UNIT_0_625_MS);

        hci_check_error(
            hci_le_set_advertising_parameters(
                interval_units, // min interval
                interval_units, // max interval
                adv_type,
                anonymous ? BT_ADDR_LE_RANDOM : BT_ADDR_LE_PUBLIC,
                &empty_addr,
                0b111,          // channel map: channels 37, 38, 39
                0x00            // filter policy: no filter
                ));

        // The HCI commands expect MAX_ADVERTISEMENT_SIZE (31)octets,
        // even though the actual data length may be shorter.
        uint8_t full_data[MAX_ADVERTISEMENT_SIZE] = { 0 };
        memcpy(full_data, advertising_data, MIN(sizeof(full_data), advertising_data_len));
        hci_check_error(hci_le_set_advertising_data(advertising_data_len, full_data));
        memset(full_data, 0, sizeof(full_data));
        if (scan_response_data_len > 0) {
            memcpy(full_data, scan_response_data, MIN(sizeof(full_data), scan_response_data_len));
            hci_check_error(hci_le_set_scan_response_data(scan_response_data_len, full_data));
        }

        // No duration mechanism is provided for legacy advertising, so we need to do our own.
        self->advertising_timeout_msecs = timeout * 1000;
        self->advertising_start_ticks = supervisor_ticks_ms64();

        // Start advertising.
        hci_check_error(hci_le_set_advertising_enable(BT_HCI_LE_ADV_ENABLE));
        self->extended_advertising = false;
    } // end legacy advertising setup

    vm_used_ble = true;
    self->now_advertising = true;
    return 0;
}

void common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self,
    bool connectable, bool anonymous, uint32_t timeout, mp_float_t interval,
    mp_buffer_info_t *advertising_data_bufinfo,
    mp_buffer_info_t *scan_response_data_bufinfo,
    mp_int_t tx_power, const bleio_address_obj_t *directed_to) {
    check_enabled(self);

    // interval value has already been validated.

    check_data_fit(advertising_data_bufinfo->len, connectable);
    check_data_fit(scan_response_data_bufinfo->len, connectable);

    if (advertising_data_bufinfo->len > MAX_ADVERTISEMENT_SIZE && scan_response_data_bufinfo->len > 0) {
        mp_raise_bleio_BluetoothError(translate("Extended advertisements with scan response not supported."));
    }

    // Anonymous mode requires a timeout so that we don't continue to broadcast
    // the same data while cycling the MAC address -- otherwise, what's the
    // point of randomizing the MAC address?
    if (timeout == 0 && anonymous) {
        timeout = MAX_ANONYMOUS_ADV_TIMEOUT_SECS;
    } else {
        if (timeout > MAX_LIMITED_DISCOVERABLE_ADV_TIMEOUT_SECS) {
            mp_raise_bleio_BluetoothError(translate("Timeout is too long: Maximum timeout length is %d seconds"),
                MAX_LIMITED_DISCOVERABLE_ADV_TIMEOUT_SECS);
        }
    }

    if (tx_power != 0) {
        mp_raise_NotImplementedError(translate("Only tx_power=0 supported"));
    }

    const uint32_t result = _common_hal_bleio_adapter_start_advertising(
        self, connectable, anonymous, timeout, interval,
        advertising_data_bufinfo->buf,
        advertising_data_bufinfo->len,
        scan_response_data_bufinfo->buf,
        scan_response_data_bufinfo->len,
        tx_power, directed_to);

    if (result) {
        mp_raise_bleio_BluetoothError(translate("Already advertising"));
    }
    self->circuitpython_advertising = false;
}

void common_hal_bleio_adapter_stop_advertising(bleio_adapter_obj_t *self) {
    check_enabled(self);

    self->now_advertising = false;
    self->extended_advertising = false;
    self->circuitpython_advertising = false;

    int result = hci_le_set_advertising_enable(BT_HCI_LE_ADV_DISABLE);
    // OK if we're already stopped. There seems to be an ESP32 HCI bug:
    // If advertising is already off, then LE_SET_ADV_ENABLE does not return a response.
    if (result != HCI_RESPONSE_TIMEOUT) {
        hci_check_error(result);
    }

    // TODO startup CircuitPython advertising again.
}

// Note that something stopped advertising, such as a connection happening.
// Don't ask the adapter to stop.
void bleio_adapter_advertising_was_stopped(bleio_adapter_obj_t *self) {
    self->now_advertising = false;
    self->extended_advertising = false;
    self->circuitpython_advertising = false;
}

bool common_hal_bleio_adapter_get_advertising(bleio_adapter_obj_t *self) {
    check_enabled(self);

    return self->now_advertising;
}

bool common_hal_bleio_adapter_get_connected(bleio_adapter_obj_t *self) {
    check_enabled(self);

    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn_handle != BLE_CONN_HANDLE_INVALID) {
            return true;
        }
    }
    return false;
}

mp_obj_t common_hal_bleio_adapter_get_connections(bleio_adapter_obj_t *self) {
    check_enabled(self);

    if (self->connection_objs != NULL) {
        return self->connection_objs;
    }
    size_t total_connected = 0;
    mp_obj_t items[BLEIO_TOTAL_CONNECTION_COUNT];
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn_handle != BLE_CONN_HANDLE_INVALID) {
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
    // TODO
    mp_raise_NotImplementedError(NULL);
    check_enabled(self);

    // FIX bonding_erase_storage();
}

uint16_t bleio_adapter_add_attribute(bleio_adapter_obj_t *adapter, mp_obj_t *attribute) {
    check_enabled(adapter);

    // The handle is the index of this attribute in the attributes list.
    uint16_t handle = (uint16_t)adapter->attributes->len;
    mp_obj_list_append(adapter->attributes, attribute);

    if (mp_obj_is_type(attribute, &bleio_service_type)) {
        adapter->last_added_service_handle = handle;
    }
    if (mp_obj_is_type(attribute, &bleio_characteristic_type)) {
        adapter->last_added_characteristic_handle = handle;
    }

    return handle;
}

mp_obj_t *bleio_adapter_get_attribute(bleio_adapter_obj_t *adapter, uint16_t handle) {
    check_enabled(adapter);

    if (handle == 0 || handle >= adapter->attributes->len) {
        return mp_const_none;
    }
    return adapter->attributes->items[handle];
}

uint16_t bleio_adapter_max_attribute_handle(bleio_adapter_obj_t *adapter) {
    check_enabled(adapter);

    return adapter->attributes->len - 1;
}


void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter) {
    gc_collect_root((void **)adapter, sizeof(bleio_adapter_obj_t) / sizeof(size_t));
    gc_collect_root((void **)bleio_connections, sizeof(bleio_connections) / sizeof(size_t));
}

void bleio_adapter_reset(bleio_adapter_obj_t *adapter) {

    if (!common_hal_bleio_adapter_get_enabled(adapter)) {
        return;
    }

    // Adapter will be reset.
    common_hal_bleio_adapter_set_enabled(adapter, false);

    adapter->connection_objs = NULL;
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        // Disconnect all connections with Python state cleanly. Keep any supervisor-only connections.
        if (connection->connection_obj != mp_const_none &&
            connection->conn_handle != BLE_CONN_HANDLE_INVALID) {
            common_hal_bleio_connection_disconnect(connection);
        }
        connection->connection_obj = mp_const_none;
    }

}

void bleio_adapter_background(bleio_adapter_obj_t *adapter) {
    if (!common_hal_bleio_adapter_get_enabled(adapter)) {
        return;
    }

    if (adapter->advertising_timeout_msecs > 0 &&
        supervisor_ticks_ms64() - adapter->advertising_start_ticks > adapter->advertising_timeout_msecs) {
        adapter->advertising_timeout_msecs = 0;
        common_hal_bleio_adapter_stop_advertising(adapter);
    }

    hci_result_t result = hci_poll_for_incoming_pkt();
    if (result != HCI_OK) {
        mp_printf(&mp_plat_print, "bad hci_poll_for_incoming_pkt() result in background: %d\n", result);
    }
}
