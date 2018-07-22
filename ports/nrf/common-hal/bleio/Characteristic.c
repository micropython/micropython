/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "ble_drv.h"
#include "ble_gatts.h"
#include "nrf_soc.h"
#include "py/nlr.h"
#include "shared-module/bleio/Characteristic.h"

static volatile bleio_characteristic_obj_t *m_read_characteristic;
static volatile uint8_t m_tx_in_progress;
static nrf_mutex_t *m_write_mutex;
//static volatile bool m_write_done;

STATIC void gatts_write(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo) {
    bleio_device_obj_t *device = characteristic->service->device;
    const uint16_t conn_handle = device->conn_handle;

    ble_gatts_value_t gatts_value = {
        .p_value = bufinfo->buf,
        .len = bufinfo->len,
    };

    const uint32_t err_code = sd_ble_gatts_value_set(conn_handle, characteristic->handle, &gatts_value);
    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
             "Failed to write gatts value, status: 0x%08lX", err_code));
    }
}

STATIC void gatts_notify(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo) {
    bleio_device_obj_t *device = characteristic->service->device;
    uint16_t hvx_len = bufinfo->len;

    ble_gatts_hvx_params_t hvx_params = {
        .handle = characteristic->handle,
        .type = BLE_GATT_HVX_NOTIFICATION,
        .p_len = &hvx_len,
        .p_data = bufinfo->buf,
    };

    while (m_tx_in_progress > MAX_TX_IN_PROGRESS) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }

    const uint32_t err_code = sd_ble_gatts_hvx(device->conn_handle, &hvx_params);
    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "Failed to notify attribute value, status: 0x%08lX", err_code));
    }

    m_tx_in_progress += 1;
}

STATIC void gattc_read(bleio_characteristic_obj_t *characteristic) {
    bleio_service_obj_t *service = characteristic->service;
    bleio_device_obj_t *device = service->device;

    m_read_characteristic = characteristic;

    const uint32_t err_code = sd_ble_gattc_read(device->conn_handle, characteristic->handle, 0);
    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Failed to read attribute value, status: 0x%08lX", err_code));
    }

    while (m_read_characteristic != NULL) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }
}

STATIC void gattc_write(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo) {
    bleio_device_obj_t *device = characteristic->service->device;
    uint16_t conn_handle = device->conn_handle;
    uint32_t err_code;

    ble_gattc_write_params_t write_params;
    write_params.write_op = BLE_GATT_OP_WRITE_REQ;

    if (characteristic->props.write_wo_resp) {
        write_params.write_op = BLE_GATT_OP_WRITE_CMD;
    }

    write_params.flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_CANCEL;
    write_params.handle   = characteristic->handle;
    write_params.offset   = 0;
    write_params.len      = bufinfo->len;
    write_params.p_value  = bufinfo->buf;

    if (write_params.write_op == BLE_GATT_OP_WRITE_CMD) {
        err_code = sd_mutex_acquire(m_write_mutex);
        if (err_code != NRF_SUCCESS) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                      "Failed to acquire mutex, status: 0x%08lX", err_code));
        }
    }

    err_code = sd_ble_gattc_write(conn_handle, &write_params);
    if (err_code != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            "Failed to write attribute value, status: 0x%08lX", err_code));
    }

    while (sd_mutex_acquire(m_write_mutex) == NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }

    err_code = sd_mutex_release(m_write_mutex);
    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Failed to release mutex, status: 0x%08lX", err_code));
    }
}

STATIC void on_ble_evt(ble_evt_t *ble_evt, void *param) {
    switch (ble_evt->header.evt_id) {
#if (BLE_API_VERSION == 4)
        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            m_tx_in_progress -= ble_evt->evt.gatts_evt.params.hvn_tx_complete.count;
            break;
#else
        case BLE_EVT_TX_COMPLETE:
            m_tx_in_progress -= ble_evt->evt.common_evt.params.tx_complete.count;
            break;
#endif

        case BLE_GATTC_EVT_READ_RSP:
        {
            ble_gattc_evt_read_rsp_t *response = &ble_evt->evt.gattc_evt.params.read_rsp;
            m_read_characteristic->value_data = mp_obj_new_bytearray(response->len, response->data);
            m_read_characteristic = NULL;
            break;
        }

        case BLE_GATTC_EVT_WRITE_RSP:
            sd_mutex_release(m_write_mutex);
//            m_write_done = true;
            break;
    }
}

void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self) {
    ble_drv_add_event_handler(on_ble_evt, NULL);
}

void common_hal_bleio_characteristic_read_value(bleio_characteristic_obj_t *self) {
    gattc_read(self);
}

void common_hal_bleio_characteristic_write_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    const bleio_device_obj_t *device = self->service->device;

    if (device->is_peripheral) {
        // TODO: Add indications
        if (self->props.notify) {
            gatts_notify(self, bufinfo);
        } else {
            gatts_write(self, bufinfo);
        }
    } else {
        gattc_write(self, bufinfo);
    }

}
