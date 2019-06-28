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

#include "py/runtime.h"
#include "common-hal/bleio/__init__.h"
#include "common-hal/bleio/Characteristic.h"
#include "shared-module/bleio/Characteristic.h"

STATIC volatile bleio_characteristic_obj_t *m_read_characteristic;
STATIC volatile uint8_t m_tx_in_progress;
// Serialize gattc writes that send a response. This might be done per object?
STATIC nrf_mutex_t *m_write_mutex;

STATIC uint16_t get_cccd(bleio_characteristic_obj_t *characteristic) {
    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);
    uint16_t cccd;
    ble_gatts_value_t value = {
        .p_value = (uint8_t*) &cccd,
        .len = 2,
    };

    const uint32_t err_code = sd_ble_gatts_value_get(conn_handle, characteristic->cccd_handle, &value);


    if (err_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING) {
        // CCCD is not set, so say that neither Notify nor Indicate is enabled.
        cccd = 0;
    } else if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to read CCCD value, err 0x%04x"), err_code);
    }

    return cccd;
}

STATIC void gatts_read(bleio_characteristic_obj_t *characteristic) {
    // This might be BLE_CONN_HANDLE_INVALID if we're not conected, but that's OK, because
    // we can still read and write the local value.
    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);

    mp_buffer_info_t bufinfo;
    ble_gatts_value_t gatts_value = {
        .p_value = NULL,
        .len = 0,
    };

    // Read once to find out what size buffer we need, then read again to fill buffer.

    uint32_t err_code = sd_ble_gatts_value_get(conn_handle, characteristic->handle, &gatts_value);
    if (err_code == NRF_SUCCESS) {
        characteristic->value_data = mp_obj_new_bytearray_of_zeros(gatts_value.len);
        mp_get_buffer_raise(characteristic->value_data, &bufinfo, MP_BUFFER_WRITE);
        gatts_value.p_value = bufinfo.buf;

        // Read again, with the correct size of buffer.
        err_code = sd_ble_gatts_value_get(conn_handle, characteristic->handle, &gatts_value);
    }

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to read gatts value, err 0x%04x"), err_code);
    }
}


STATIC void gatts_write(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo) {
    // This might be BLE_CONN_HANDLE_INVALID if we're not conected, but that's OK, because
    // we can still read and write the local value.
    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);

    ble_gatts_value_t gatts_value = {
        .p_value = bufinfo->buf,
        .len = bufinfo->len,
    };

    const uint32_t err_code = sd_ble_gatts_value_set(conn_handle, characteristic->handle, &gatts_value);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to write gatts value, err 0x%04x"), err_code);
    }
}

STATIC void gatts_notify_indicate(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo, uint16_t hvx_type) {
    uint16_t hvx_len = bufinfo->len;

    ble_gatts_hvx_params_t hvx_params = {
        .handle = characteristic->handle,
        .type = hvx_type,
        .offset = 0,
        .p_len = &hvx_len,
        .p_data = bufinfo->buf,
    };

    while (m_tx_in_progress >= MAX_TX_IN_PROGRESS) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }

    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);
    m_tx_in_progress++;
    const uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
    if (err_code != NRF_SUCCESS) {
        m_tx_in_progress--;
        mp_raise_OSError_msg_varg(translate("Failed to notify or indicate attribute value, err 0x%04x"), err_code);
    }

}

STATIC void gattc_read(bleio_characteristic_obj_t *characteristic) {
    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);

    m_read_characteristic = characteristic;

    const uint32_t err_code = sd_ble_gattc_read(conn_handle, characteristic->handle, 0);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to read attribute value, err 0x%04x"), err_code);
    }

//
    while (m_read_characteristic != NULL) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }
}

STATIC void gattc_write(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo) {
    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);
    uint32_t err_code;

    ble_gattc_write_params_t write_params = {
        .flags = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_CANCEL,
        .write_op = BLE_GATT_OP_WRITE_REQ,
        .handle = characteristic->handle,
        .p_value = bufinfo->buf,
        .len = bufinfo->len,
    };

    if (characteristic->props.write_no_response) {
        write_params.write_op = BLE_GATT_OP_WRITE_CMD;

        err_code = sd_mutex_acquire(m_write_mutex);
        if (err_code != NRF_SUCCESS) {
            mp_raise_OSError_msg_varg(translate("Failed to acquire mutex, err 0x%04x"), err_code);
        }
    }

    err_code = sd_ble_gattc_write(conn_handle, &write_params);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to write attribute value, err 0x%04x"), err_code);
    }

    while (sd_mutex_acquire(m_write_mutex) == NRF_ERROR_SOC_MUTEX_ALREADY_TAKEN) {
#ifdef MICROPY_VM_HOOK_LOOP
    MICROPY_VM_HOOK_LOOP
#endif
    }

    err_code = sd_mutex_release(m_write_mutex);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to release mutex, err 0x%04x"), err_code);
    }
}

STATIC void characteristic_on_ble_evt(ble_evt_t *ble_evt, void *param) {
    switch (ble_evt->header.evt_id) {
    case BLE_GATTS_EVT_HVN_TX_COMPLETE:
    {
        uint8_t count = ble_evt->evt.gatts_evt.params.hvn_tx_complete.count;
        // Don't underflow the count.
        if (count >= m_tx_in_progress) {
            m_tx_in_progress = 0;
        } else {
            m_tx_in_progress -= count;
        }
        break;
    }

    case BLE_GATTC_EVT_READ_RSP:
    {
        ble_gattc_evt_read_rsp_t *response = &ble_evt->evt.gattc_evt.params.read_rsp;
        m_read_characteristic->value_data = mp_obj_new_bytearray(response->len, response->data);
        // Flag to busy-wait loop that we've read the characteristic.
        m_read_characteristic = NULL;
        break;
    }

    case BLE_GATTC_EVT_WRITE_RSP:
        // Someone else can write now.
        sd_mutex_release(m_write_mutex);
        break;

        // For debugging.
    default:
        // mp_printf(&mp_plat_print, "Unhandled characteristic event: 0x%04x\n", ble_evt->header.evt_id);
        break;
    }

}

void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self, bleio_uuid_obj_t *uuid, bleio_characteristic_properties_t props) {
    self->service = NULL;
    self->uuid = uuid;
    self->value_data = NULL;
    self->props = props;
    self->handle = BLE_GATT_HANDLE_INVALID;

    ble_drv_add_event_handler(characteristic_on_ble_evt, self);

}

void common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self) {
    switch (common_hal_bleio_device_get_gatt_role(self->service->device)) {
    case GATT_ROLE_CLIENT:
        gattc_read(self);
        break;

    case GATT_ROLE_SERVER:
        gatts_read(self);
        break;

    default:
        mp_raise_RuntimeError(translate("bad GATT role"));
        break;
    }
}

void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    bool sent = false;
    uint16_t cccd = 0;

    switch (common_hal_bleio_device_get_gatt_role(self->service->device)) {
    case GATT_ROLE_SERVER:
        if (self->props.notify || self->props.indicate) {
            cccd = get_cccd(self);
        }
        // It's possible that both notify and indicate are set.
        if (self->props.notify && (cccd & BLE_GATT_HVX_NOTIFICATION)) {
            gatts_notify_indicate(self, bufinfo, BLE_GATT_HVX_NOTIFICATION);
            sent = true;
        }
        if (self->props.indicate && (cccd & BLE_GATT_HVX_INDICATION)) {
            gatts_notify_indicate(self, bufinfo, BLE_GATT_HVX_INDICATION);
            sent = true;
        }
        if (!sent) {
            gatts_write(self, bufinfo);
        }
        break;

    case GATT_ROLE_CLIENT:
        gattc_write(self, bufinfo);
        break;

    default:
        mp_raise_RuntimeError(translate("bad GATT role"));
        break;
    }
}
