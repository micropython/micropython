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
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Adapter.h"
#include "shared-bindings/_bleio/Central.h"

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
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            sd_ble_gap_sec_params_reply(central->conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: {
            ble_gap_evt_conn_param_update_request_t *request =
                &ble_evt->evt.gap_evt.params.conn_param_update_request;
            sd_ble_gap_conn_param_update(central->conn_handle, &request->conn_params);
            break;
        }
        default:
            // For debugging.
            // mp_printf(&mp_plat_print, "Unhandled central event: 0x%04x\n", ble_evt->header.evt_id);
            break;
    }
}

void common_hal_bleio_central_construct(bleio_central_obj_t *self) {
    common_hal_bleio_adapter_set_enabled(true);

    self->remote_service_list = mp_obj_new_list(0, NULL);
    self->conn_handle = BLE_CONN_HANDLE_INVALID;
}

void common_hal_bleio_central_connect(bleio_central_obj_t *self, bleio_address_obj_t *address, mp_float_t timeout) {
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
        RUN_BACKGROUND_TASKS;
    }

    if (self->conn_handle == BLE_CONN_HANDLE_INVALID) {
        mp_raise_OSError_msg(translate("Failed to connect: timeout"));
    }
}

void common_hal_bleio_central_disconnect(bleio_central_obj_t *self) {
    sd_ble_gap_disconnect(self->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}

bool common_hal_bleio_central_get_connected(bleio_central_obj_t *self) {
    return self->conn_handle != BLE_CONN_HANDLE_INVALID;
}

mp_obj_tuple_t *common_hal_bleio_central_discover_remote_services(bleio_central_obj_t *self, mp_obj_t service_uuids_whitelist) {
    common_hal_bleio_device_discover_remote_services(MP_OBJ_FROM_PTR(self), service_uuids_whitelist);
    // Convert to a tuple and then clear the list so the callee will take ownership.
    mp_obj_tuple_t *services_tuple = mp_obj_new_tuple(self->remote_service_list->len,
                                                      self->remote_service_list->items);
    mp_obj_list_clear(self->remote_service_list);
    return services_tuple;
}

mp_obj_list_t *common_hal_bleio_central_get_remote_services(bleio_central_obj_t *self) {
    return self->remote_service_list;
}
