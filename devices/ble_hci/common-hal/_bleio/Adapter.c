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

#include "bonding.h"

#include "py/gc.h"
#include "py/objstr.h"
#include "py/runtime.h"
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

#define BLE_MIN_CONN_INTERVAL        MSEC_TO_UNITS(15, UNIT_0_625_MS)
#define BLE_MAX_CONN_INTERVAL        MSEC_TO_UNITS(15, UNIT_0_625_MS)
#define BLE_SLAVE_LATENCY            0
#define BLE_CONN_SUP_TIMEOUT         MSEC_TO_UNITS(4000, UNIT_10_MS)

#ifndef BLEIO_VS_UUID_COUNT
#define BLEIO_VS_UUID_COUNT 75
#endif

#ifndef BLEIO_HVN_TX_QUEUE_SIZE
#define BLEIO_HVN_TX_QUEUE_SIZE 9
#endif

#ifndef BLEIO_CENTRAL_ROLE_COUNT
#define BLEIO_CENTRAL_ROLE_COUNT 4
#endif

#ifndef BLEIO_PERIPH_ROLE_COUNT
#define BLEIO_PERIPH_ROLE_COUNT 4
#endif

#ifndef BLEIO_ATTR_TAB_SIZE
#define BLEIO_ATTR_TAB_SIZE (BLE_GATTS_ATTR_TAB_SIZE_DEFAULT * 5)
#endif

bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

STATIC bool adapter_on_ble_evt(ble_evt_t *ble_evt, void *self_in) {
    // bleio_adapter_obj_t *self = (bleio_adapter_obj_t*)self_in;

    // // For debugging.
    // // mp_printf(&mp_plat_print, "Adapter event: 0x%04x\n", ble_evt->header.evt_id);

    // switch (ble_evt->header.evt_id) {
    //     case BLE_GAP_EVT_CONNECTED: {
    //         // Find an empty connection. One must always be available because the SD has the same
    //         // total connection limit.
    //         bleio_connection_internal_t *connection;
    //         for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
    //             connection = &bleio_connections[i];
    //             if (connection->conn_handle == BLE_CONN_HANDLE_INVALID) {
    //                 break;
    //             }
    //         }

    //         // Central has connected.
    //         ble_gap_evt_connected_t* connected = &ble_evt->evt.gap_evt.params.connected;

    //         connection->conn_handle = ble_evt->evt.gap_evt.conn_handle;
    //         connection->connection_obj = mp_const_none;
    //         connection->pair_status = PAIR_NOT_PAIRED;
    //         connection->mtu = 0;

    //         ble_drv_add_event_handler_entry(&connection->handler_entry, connection_on_ble_evt, connection);
    //         self->connection_objs = NULL;

    //         // Save the current connection parameters.
    //         memcpy(&connection->conn_params, &connected->conn_params, sizeof(ble_gap_conn_params_t));

    //         #if CIRCUITPY_VERBOSE_BLE
    //         ble_gap_conn_params_t *cp = &connected->conn_params;
    //         mp_printf(&mp_plat_print, "conn params: min_ci %d max_ci %d s_l %d sup_timeout %d\n", cp->min_conn_interval, cp->max_conn_interval, cp->slave_latency, cp->conn_sup_timeout);
    //         #endif

    //         // See if connection interval set by Central is out of range.
    //         // If so, negotiate our preferred range.
    //         ble_gap_conn_params_t conn_params;
    //         sd_ble_gap_ppcp_get(&conn_params);
    //         if (conn_params.min_conn_interval < connected->conn_params.min_conn_interval ||
    //             conn_params.min_conn_interval > connected->conn_params.max_conn_interval) {
    //             sd_ble_gap_conn_param_update(ble_evt->evt.gap_evt.conn_handle, &conn_params);
    //         }
    //         self->current_advertising_data = NULL;
    //         break;
    //     }
    //     case BLE_GAP_EVT_DISCONNECTED: {
    //         // Find the connection that was disconnected.
    //         bleio_connection_internal_t *connection;
    //         for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
    //             connection = &bleio_connections[i];
    //             if (connection->conn_handle == ble_evt->evt.gap_evt.conn_handle) {
    //                 break;
    //             }
    //         }
    //         ble_drv_remove_event_handler(connection_on_ble_evt, connection);
    //         connection->conn_handle = BLE_CONN_HANDLE_INVALID;
    //         connection->pair_status = PAIR_NOT_PAIRED;
    //         if (connection->connection_obj != mp_const_none) {
    //             bleio_connection_obj_t* obj = connection->connection_obj;
    //             obj->connection = NULL;
    //             obj->disconnect_reason = ble_evt->evt.gap_evt.params.disconnected.reason;
    //         }
    //         self->connection_objs = NULL;

    //         break;
    //     }

    //     case BLE_GAP_EVT_ADV_SET_TERMINATED:
    //         self->current_advertising_data = NULL;
    //         break;

    //     default:
    //         // For debugging.
    //         // mp_printf(&mp_plat_print, "Unhandled adapter event: 0x%04x\n", ble_evt->header.evt_id);
    //         return false;
    //         break;
    // }
    return true;
}

STATIC void get_address(bleio_adapter_obj_t *self, ble_gap_addr_t *address) {
//    check_nrf_error(sd_ble_gap_addr_get(address));
}

char default_ble_name[] = { 'C', 'I', 'R', 'C', 'U', 'I', 'T', 'P', 'Y', 0, 0, 0, 0 , 0};

STATIC void bleio_adapter_reset_name(bleio_adapter_obj_t *self) {
    uint8_t len = sizeof(default_ble_name) - 1;

    ble_gap_addr_t local_address;
    get_address(self, &local_address);

    default_ble_name[len - 4] = nibble_to_hex_lower[local_address.addr[1] >> 4 & 0xf];
    default_ble_name[len - 3] = nibble_to_hex_lower[local_address.addr[1] & 0xf];
    default_ble_name[len - 2] = nibble_to_hex_lower[local_address.addr[0] >> 4 & 0xf];
    default_ble_name[len - 1] = nibble_to_hex_lower[local_address.addr[0] & 0xf];
    default_ble_name[len] = '\0'; // for now we add null for compatibility with C ASCIIZ strings

    common_hal_bleio_adapter_set_name(self, (char*) default_ble_name);
}

void common_hal_bleio_adapter_construct(bleio_adapter_obj_t *self, mcu_pin_obj_t *tx, mcu_pin_obj_t *rx, mcu_pin_obj_t *rts, mcu_pin_obj_t *cts, uint32_t baudrate, uint32_t buffer_size, mcu_pin_obj_t* spi_cs, mcu_pin_obj_t* gpio0, mcu_pin_obj_t *reset, bool reset_high) {
    self->tx = tx;
    self->rx = rx;
    self->rts = rts;
    self->cts = cts;
    self->baudrate = baudrate;
    self->buffer_size = buffer_size;
    self->spi_cs = spi_cs;
    self->gpio0 = gpio0;
    self->reset = reset;
    self->reset_high = reset_high;
    self->enabled = false;
}

void common_hal_bleio_adapter_set_enabled(bleio_adapter_obj_t *self, bool enabled) {
    const bool is_enabled = common_hal_bleio_adapter_get_enabled(self);

    // Don't enable or disable twice
    if (is_enabled == enabled) {
        return;
    }

    if (enabled) {
        // Enable adapter.

        // common_hal UART takes rts and cts, but is currently not implemented for many ports.
        // In addition, rts and cts may be pins that are not part of the serial peripheral
        // used for tx and rx, so use GPIO for them.
        common_hal_busio_uart_construct(&self->hci_uart, tx, rx, NULL, NULL, NULL, false,
                                        BLEIO_HCI_BAUDRATE, 8, PARITY_NONE, 1, 0.0f,
                                        BLEIO_HCI_BUFFER_SIZE, NULL, false);

        // RTS is output, active high
        common_hal_digitalio_digitalinout_construct(&self->rts_digitalio, self->rts);
        common_hal_digitalio_digitalinout_switch_to_output(&self->rts_digitalio, false, DRIVE_MODE_PUSH_PULL);

        // CTS is input.
        common_hal_digitalio_digitalinout_construct(&self->cts_digitalio, self->cts);

        // SPI_CS and GPI0 are used to signal entering BLE mode.
        // SPI_CS should be low, and GPI0 should be high
        common_hal_digitalio_digitalinout_construct(&self->spi_cs_digitalio, self->spi_cs);
        common_hal_digitalio_digitalinout_construct(&self->gpio0_digitalio, self->gpi0);
        common_hal_digitalio_digitalinout_switch_to_output(&self->spi_cs_digitalio, false, DRIVE_MODE_PUSH_PULL);
        common_hal_digitalio_digitalinout_switch_to_output(&self->gpio0_digitalio, true DRIVE_MODE_PUSH_PULL);

        // RESET is output, start in non-reset state.
        common_hal_digitalio_digitalinout_construct(&self->reset_digitalio, self->reset);
        common_hal_digitalio_digitalinout_switch_to_output(&self->reset_digitalio,
                                                           !self->reset_high, DRIVE_MODE_PUSH_PULL);

        // Adapter will enter BLE mode on reset, based on SPI_CS and GPIO0 settings.
        // Reset HCI processor. Assert reset for 100ms, then wait 750ms for reset to complete.
        common_hal_digitalio_digitalinout_set_value(&self->reset_digitalio, self->reset_high);
        mp_hal_delay_ms(100);
        common_hal_digitalio_digitalinout_set_value(&self->reset_digitalio, !self->reset_high);
        mp_hal_delay_ms(750);

        // After reset, set SPI_CS high.
        common_hal_digitalio_digitalinout_set_value(&self->spi_cs_digitalio, true);

        return;
    }

    // Disable.
    common_hal_digitalio_digitalinout_set_value(&self->reset_digitalio, self->reset_high);
    mp_hal_delay_ms(100);
    common_hal_digitalio_digitalinout_set_value(&self->reset_digitalio, !self->reset_high);

    // Free all pins.
    common_hal_busio_uart_deinit(&self->hci_uart);
    common_hal_digitalio_digitalinout_deinit(&self->rts_digitalio);
    common_hal_digitalio_digitalinout_deinit(&self->cts_digitalio);
    common_hal_digitalio_digitalinout_deinit(&self->spi_cs_digitalio);
    common_hal_digitalio_digitalinout_deinit(&self->gpi0_digitalio);
    common_hal_digitalio_digitalinout_deinit(&self->reset_digitalio);
}

bool common_hal_bleio_adapter_get_enabled(bleio_adapter_obj_t *self) {
    return self->enabled;
}

bleio_address_obj_t *common_hal_bleio_adapter_get_address(bleio_adapter_obj_t *self) {
    common_hal_bleio_adapter_set_enabled(self, true);

    ble_gap_addr_t local_address;
    get_address(self, &local_address);

    bleio_address_obj_t *address = m_new_obj(bleio_address_obj_t);
    address->base.type = &bleio_address_type;

    common_hal_bleio_address_construct(address, local_address.addr, local_address.addr_type);
    return address;
}

mp_obj_str_t* common_hal_bleio_adapter_get_name(bleio_adapter_obj_t *self) {
    uint16_t len = 0;
//    sd_ble_gap_device_name_get(NULL, &len);
    uint8_t buf[len];
//    uint32_t err_code = sd_ble_gap_device_name_get(buf, &len);
//    if (err_code != NRF_SUCCESS) {
//        return NULL;
//    }
    return mp_obj_new_str((char*) buf, len);
}

void common_hal_bleio_adapter_set_name(bleio_adapter_obj_t *self, const char* name) {
    // ble_gap_conn_sec_mode_t sec;
    // sec.lv = 0;
    // sec.sm = 0;
    // sd_ble_gap_device_name_set(&sec, (const uint8_t*) name, strlen(name));
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

mp_obj_t common_hal_bleio_adapter_start_scan(bleio_adapter_obj_t *self, uint8_t* prefixes, size_t prefix_length, bool extended, mp_int_t buffer_size, mp_float_t timeout, mp_float_t interval, mp_float_t window, mp_int_t minimum_rssi, bool active) {
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
    // sd_ble_gap_scan_stop();
    shared_module_bleio_scanresults_set_done(self->scan_results, true);
    // ble_drv_remove_event_handler(scan_on_ble_evt, self->scan_results);
    self->scan_results = NULL;
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
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn_handle == conn_handle) {
            return bleio_connection_new_from_internal(connection);
        }
    }

    mp_raise_bleio_BluetoothError(translate("Failed to connect: internal error"));

    return mp_const_none;
}

// The nRF SD 6.1.0 can only do one concurrent advertisement so share the advertising handle.
uint8_t adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

STATIC void check_data_fit(size_t data_len, bool connectable) {
    if (data_len > BLE_GAP_ADV_SET_DATA_SIZE_EXTENDED_MAX_SUPPORTED ||
        (connectable && data_len > BLE_GAP_ADV_SET_DATA_SIZE_EXTENDED_CONNECTABLE_MAX_SUPPORTED)) {
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

uint32_t _common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self, bool connectable, bool anonymous, uint32_t timeout, float interval, uint8_t *advertising_data, uint16_t advertising_data_len, uint8_t *scan_response_data, uint16_t scan_response_data_len) {
    // if (self->current_advertising_data != NULL && self->current_advertising_data == self->advertising_data) {
    //     return NRF_ERROR_BUSY;
    // }

    // // If the current advertising data isn't owned by the adapter then it must be an internal
    // // advertisement that we should stop.
    // if (self->current_advertising_data != NULL) {
    //     common_hal_bleio_adapter_stop_advertising(self);
    // }

    // uint32_t err_code;
    // bool extended = advertising_data_len > BLE_GAP_ADV_SET_DATA_SIZE_MAX ||
    //                 scan_response_data_len > BLE_GAP_ADV_SET_DATA_SIZE_MAX;

    // uint8_t adv_type;
    // if (extended) {
    //     if (connectable) {
    //         adv_type = BLE_GAP_ADV_TYPE_EXTENDED_CONNECTABLE_NONSCANNABLE_UNDIRECTED;
    //     } else if (scan_response_data_len > 0) {
    //         adv_type = BLE_GAP_ADV_TYPE_EXTENDED_NONCONNECTABLE_SCANNABLE_UNDIRECTED;
    //     } else {
    //         adv_type = BLE_GAP_ADV_TYPE_EXTENDED_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
    //     }
    // } else if (connectable) {
    //     adv_type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    // } else if (scan_response_data_len > 0) {
    //     adv_type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED;
    // } else {
    //     adv_type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
    // }

    // if (anonymous) {
    //     ble_gap_privacy_params_t privacy = {
    //         .privacy_mode = BLE_GAP_PRIVACY_MODE_DEVICE_PRIVACY,
    //         .private_addr_type = BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE,
    //         // Rotate the keys one second after we're scheduled to stop
    //         // advertising. This prevents a potential race condition where we
    //         // fire off a beacon with the same advertising data but a new MAC
    //         // address just as we tear down the connection.
    //         .private_addr_cycle_s = timeout + 1,
    //         .p_device_irk = NULL,
    //     };
    //     err_code = sd_ble_gap_privacy_set(&privacy);
    // } else {
    //     ble_gap_privacy_params_t privacy = {
    //         .privacy_mode = BLE_GAP_PRIVACY_MODE_OFF,
    //         .private_addr_type = BLE_GAP_ADDR_TYPE_PUBLIC,
    //         .private_addr_cycle_s = 0,
    //         .p_device_irk = NULL,
    //     };
    //     err_code = sd_ble_gap_privacy_set(&privacy);
    // }
    // if (err_code != NRF_SUCCESS) {
    //     return err_code;
    // }

    // ble_gap_adv_params_t adv_params = {
    //     .interval = SEC_TO_UNITS(interval, UNIT_0_625_MS),
    //     .properties.type = adv_type,
    //     .duration = SEC_TO_UNITS(timeout, UNIT_10_MS),
    //     .filter_policy = BLE_GAP_ADV_FP_ANY,
    //     .primary_phy = BLE_GAP_PHY_1MBPS,
    // };

    // const ble_gap_adv_data_t ble_gap_adv_data = {
    //     .adv_data.p_data = advertising_data,
    //     .adv_data.len = advertising_data_len,
    //     .scan_rsp_data.p_data = scan_response_data_len > 0 ? scan_response_data : NULL,
    //     .scan_rsp_data.len = scan_response_data_len,
    // };

    // err_code = sd_ble_gap_adv_set_configure(&adv_handle, &ble_gap_adv_data, &adv_params);
    // if (err_code != NRF_SUCCESS) {
    //     return err_code;
    // }

    // ble_drv_add_event_handler(advertising_on_ble_evt, self);

    // vm_used_ble = true;
    // err_code = sd_ble_gap_adv_start(adv_handle, BLE_CONN_CFG_TAG_CUSTOM);
    // if (err_code != NRF_SUCCESS) {
    //     return err_code;
    // }
    // self->current_advertising_data = advertising_data;
    // return NRF_SUCCESS;
    return 0;
}


void common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self, bool connectable, bool anonymous, uint32_t timeout, mp_float_t interval, mp_buffer_info_t *advertising_data_bufinfo, mp_buffer_info_t *scan_response_data_bufinfo) {
    if (self->current_advertising_data != NULL && self->current_advertising_data == self->advertising_data) {
        mp_raise_bleio_BluetoothError(translate("Already advertising."));
    }
    // interval value has already been validated.

    check_data_fit(advertising_data_bufinfo->len, connectable);
    check_data_fit(scan_response_data_bufinfo->len, connectable);

    if (advertising_data_bufinfo->len > 31 && scan_response_data_bufinfo->len > 0) {
        mp_raise_bleio_BluetoothError(translate("Extended advertisements with scan response not supported."));
    }

    // Anonymous mode requires a timeout so that we don't continue to broadcast
    // the same data while cycling the MAC address -- otherwise, what's the
    // point of randomizing the MAC address?
    if (!timeout) {
        if (anonymous) {
            // The Nordic macro is in units of 10ms. Convert to seconds.
            uint32_t adv_timeout_max_secs = UNITS_TO_SEC(BLE_GAP_ADV_TIMEOUT_LIMITED_MAX, UNIT_10_MS);
            uint32_t rotate_timeout_max_secs = BLE_GAP_DEFAULT_PRIVATE_ADDR_CYCLE_INTERVAL_S;
            timeout = MIN(adv_timeout_max_secs, rotate_timeout_max_secs);
        }
        else {
            timeout = BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED;
        }
    } else {
        if (SEC_TO_UNITS(timeout, UNIT_10_MS) > BLE_GAP_ADV_TIMEOUT_LIMITED_MAX) {
            mp_raise_bleio_BluetoothError(translate("Timeout is too long: Maximum timeout length is %d seconds"),
                                        UNITS_TO_SEC(BLE_GAP_ADV_TIMEOUT_LIMITED_MAX, UNIT_10_MS));
        }
    }

    // The advertising data buffers must not move, because the SoftDevice depends on them.
    // So make them long-lived and reuse them onwards.
    if (self->advertising_data == NULL) {
        self->advertising_data = (uint8_t *) gc_alloc(BLE_GAP_ADV_SET_DATA_SIZE_EXTENDED_MAX_SUPPORTED * sizeof(uint8_t), false, true);
    }
    if (self->scan_response_data == NULL) {
        self->scan_response_data = (uint8_t *) gc_alloc(BLE_GAP_ADV_SET_DATA_SIZE_EXTENDED_MAX_SUPPORTED * sizeof(uint8_t), false, true);
    }

    memcpy(self->advertising_data, advertising_data_bufinfo->buf, advertising_data_bufinfo->len);
    memcpy(self->scan_response_data, scan_response_data_bufinfo->buf, scan_response_data_bufinfo->len);

    // check_nrf_error(_common_hal_bleio_adapter_start_advertising(self, connectable, anonymous, timeout, interval,
    //                                                             self->advertising_data,
    //                                                             advertising_data_bufinfo->len,
    //                                                             self->scan_response_data,
    //                                                             scan_response_data_bufinfo->len));
}

void common_hal_bleio_adapter_stop_advertising(bleio_adapter_obj_t *self) {
    // if (adv_handle == BLE_GAP_ADV_SET_HANDLE_NOT_SET)
    //     return;

    // // TODO: Don't actually stop. Switch to advertising CircuitPython if we don't already have a connection.
    // const uint32_t err_code = sd_ble_gap_adv_stop(adv_handle);
    // self->current_advertising_data = NULL;

    // if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_INVALID_STATE)) {
    //     check_nrf_error(err_code);
    // }
}

bool common_hal_bleio_adapter_get_advertising(bleio_adapter_obj_t *self) {
    return self->current_advertising_data != NULL;
}

bool common_hal_bleio_adapter_get_connected(bleio_adapter_obj_t *self) {
    for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
        bleio_connection_internal_t *connection = &bleio_connections[i];
        if (connection->conn_handle != BLE_CONN_HANDLE_INVALID) {
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
    bonding_erase_storage();
}

void bleio_adapter_gc_collect(bleio_adapter_obj_t* adapter) {
    gc_collect_root((void**)adapter, sizeof(bleio_adapter_obj_t) / sizeof(size_t));
    gc_collect_root((void**)bleio_connections, sizeof(bleio_connections) / sizeof(size_t));
}

void bleio_adapter_reset(bleio_adapter_obj_t* adapter) {
    common_hal_bleio_adapter_stop_scan(adapter);
    if (adapter->current_advertising_data != NULL) {
        common_hal_bleio_adapter_stop_advertising(adapter);
    }

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
