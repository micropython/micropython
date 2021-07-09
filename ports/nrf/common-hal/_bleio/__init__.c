/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2016 Glenn Ruben Bakke
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
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"
#include "supervisor/shared/bluetooth/bluetooth.h"

#include "common-hal/_bleio/__init__.h"
#include "common-hal/_bleio/bonding.h"

void check_nrf_error(uint32_t err_code) {
    if (err_code == NRF_SUCCESS) {
        return;
    }
    switch (err_code) {
        case NRF_ERROR_NO_MEM:
            mp_raise_msg(&mp_type_MemoryError, translate("Nordic system firmware out of memory"));
            return;
        case NRF_ERROR_TIMEOUT:
            mp_raise_msg(&mp_type_TimeoutError, NULL);
            return;
        case NRF_ERROR_INVALID_PARAM:
            mp_raise_ValueError(translate("Invalid BLE parameter"));
            return;
        case BLE_ERROR_INVALID_CONN_HANDLE:
            mp_raise_ConnectionError(translate("Not connected"));
            return;
        default:
            mp_raise_bleio_BluetoothError(translate("Unknown system firmware error: %04x"), err_code);
            break;
    }
}

void check_gatt_status(uint16_t gatt_status) {
    if (gatt_status == BLE_GATT_STATUS_SUCCESS) {
        return;
    }
    switch (gatt_status) {
        case BLE_GATT_STATUS_ATTERR_INSUF_AUTHENTICATION:
            mp_raise_bleio_SecurityError(translate("Insufficient authentication"));
            return;
        case BLE_GATT_STATUS_ATTERR_INSUF_ENCRYPTION:
            mp_raise_bleio_SecurityError(translate("Insufficient encryption"));
            return;
        default:
            mp_raise_bleio_BluetoothError(translate("Unknown gatt error: 0x%04x"), gatt_status);
    }
}

void check_sec_status(uint8_t sec_status) {
    if (sec_status == BLE_GAP_SEC_STATUS_SUCCESS) {
        return;
    }

    switch (sec_status) {
        case BLE_GAP_SEC_STATUS_UNSPECIFIED:
            mp_raise_bleio_SecurityError(translate("Unspecified issue. Can be that the pairing prompt on the other device was declined or ignored."));
            return;
        default:
            mp_raise_bleio_SecurityError(translate("Unknown security error: 0x%04x"), sec_status);
    }
}

// Turn off BLE on a reset or reload.
void bleio_reset() {
    // Set this explicitly to save data.
    common_hal_bleio_adapter_obj.base.type = &bleio_adapter_type;
    if (!common_hal_bleio_adapter_get_enabled(&common_hal_bleio_adapter_obj)) {
        return;
    }

    bleio_adapter_reset(&common_hal_bleio_adapter_obj);
    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, false);
    bonding_reset();
    supervisor_start_bluetooth();
}

// The singleton _bleio.Adapter object, bound to _bleio.adapter
// It currently only has properties and no state. Inited by bleio_reset
bleio_adapter_obj_t common_hal_bleio_adapter_obj;

void common_hal_bleio_check_connected(uint16_t conn_handle) {
    if (conn_handle == BLE_CONN_HANDLE_INVALID) {
        mp_raise_ConnectionError(translate("Not connected"));
    }
}

// GATTS read of a Characteristic or Descriptor.
size_t common_hal_bleio_gatts_read(uint16_t handle, uint16_t conn_handle, uint8_t *buf, size_t len) {
    // conn_handle is ignored unless this is a system attribute.
    // If we're not connected, that's OK, because we can still read and write the local value.

    ble_gatts_value_t gatts_value = {
        .p_value = buf,
        .len = len,
    };

    check_nrf_error(sd_ble_gatts_value_get(conn_handle, handle, &gatts_value));

    return gatts_value.len;
}

void common_hal_bleio_gatts_write(uint16_t handle, uint16_t conn_handle, mp_buffer_info_t *bufinfo) {
    // conn_handle is ignored unless this is a system attribute.
    // If we're not connected, that's OK, because we can still read and write the local value.

    ble_gatts_value_t gatts_value = {
        .p_value = bufinfo->buf,
        .len = bufinfo->len,
    };

    check_nrf_error(sd_ble_gatts_value_set(conn_handle, handle, &gatts_value));
}

typedef struct {
    uint8_t *buf;
    size_t len;
    size_t final_len;
    uint16_t conn_handle;
    volatile uint16_t status;
    volatile bool done;
} read_info_t;

STATIC bool _on_gattc_read_rsp_evt(ble_evt_t *ble_evt, void *param) {
    read_info_t *read = param;
    switch (ble_evt->header.evt_id) {

        // More events may be handled later, so keep this as a switch.

        case BLE_GATTC_EVT_READ_RSP: {
            ble_gattc_evt_t *evt = &ble_evt->evt.gattc_evt;
            ble_gattc_evt_read_rsp_t *response = &evt->params.read_rsp;
            if (read && evt->conn_handle == read->conn_handle) {
                read->status = evt->gatt_status;
                size_t len = MIN(read->len, response->len);
                memcpy(read->buf, response->data, len);
                read->final_len = len;
                // Indicate to busy-wait loop that we've read the attribute value.
                read->done = true;
            }
            break;
        }

        default:
            // For debugging.
            // mp_printf(&mp_plat_print, "Unhandled characteristic event: 0x%04x\n", ble_evt->header.evt_id);
            return false;
            break;
    }
    return true;
}

size_t common_hal_bleio_gattc_read(uint16_t handle, uint16_t conn_handle, uint8_t *buf, size_t len) {
    common_hal_bleio_check_connected(conn_handle);

    read_info_t read_info;
    read_info.buf = buf;
    read_info.len = len;
    read_info.final_len = 0;
    read_info.conn_handle = conn_handle;
    // Set to true by the event handler.
    read_info.done = false;
    ble_drv_add_event_handler(_on_gattc_read_rsp_evt, &read_info);

    uint32_t nrf_error = NRF_ERROR_BUSY;
    while (nrf_error == NRF_ERROR_BUSY) {
        nrf_error = sd_ble_gattc_read(conn_handle, handle, 0);
    }
    if (nrf_error != NRF_SUCCESS) {
        ble_drv_remove_event_handler(_on_gattc_read_rsp_evt, &read_info);
        check_nrf_error(nrf_error);
    }

    while (!read_info.done) {
        RUN_BACKGROUND_TASKS;
    }

    ble_drv_remove_event_handler(_on_gattc_read_rsp_evt, &read_info);
    check_gatt_status(read_info.status);
    return read_info.final_len;
}

void common_hal_bleio_gattc_write(uint16_t handle, uint16_t conn_handle, mp_buffer_info_t *bufinfo, bool write_no_response) {
    common_hal_bleio_check_connected(conn_handle);

    ble_gattc_write_params_t write_params = {
        .write_op = write_no_response ? BLE_GATT_OP_WRITE_CMD: BLE_GATT_OP_WRITE_REQ,
        .handle = handle,
        .p_value = bufinfo->buf,
        .len = bufinfo->len,
    };

    while (1) {
        uint32_t err_code = sd_ble_gattc_write(conn_handle, &write_params);
        if (err_code == NRF_SUCCESS) {
            break;
        }

        // Write with response will return NRF_ERROR_BUSY if the response has not been received.
        // Write without reponse will return NRF_ERROR_RESOURCES if too many writes are pending.
        if (err_code == NRF_ERROR_BUSY || err_code == NRF_ERROR_RESOURCES) {
            // We could wait for an event indicating the write is complete, but just retrying is easier.
            MICROPY_VM_HOOK_LOOP;
            continue;
        }

        // Some real error occurred.
        check_nrf_error(err_code);
    }

}

void bleio_background(void) {
    bonding_background();
}

void common_hal_bleio_gc_collect(void) {
    bleio_adapter_gc_collect(&common_hal_bleio_adapter_obj);
}
