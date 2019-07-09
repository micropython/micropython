/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) Dan Halbert for Adafruit Industries
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

STATIC volatile bleio_characteristic_obj_t *m_read_characteristic;

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
    // This might be BLE_CONN_HANDLE_INVALID if we're not connected, but that's OK, because
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

    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);

    while (1) {
        const uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
        if (err_code == NRF_SUCCESS) {
            break;
        }
        // TX buffer is full
        // We could wait for an event indicating the write is complete, but just retrying is easier.
        if (err_code == NRF_ERROR_RESOURCES) {
            MICROPY_VM_HOOK_LOOP;
            continue;
        }

        // Some real error has occurred.
        mp_raise_OSError_msg_varg(translate("Failed to notify or indicate attribute value, err 0x%04x"), err_code);
    }

}

STATIC void check_connected(uint16_t conn_handle) {
    if (conn_handle == BLE_CONN_HANDLE_INVALID) {
        mp_raise_OSError_msg(translate("Not connected"));
    }
}

STATIC void gattc_read(bleio_characteristic_obj_t *characteristic) {
    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);
    check_connected(conn_handle);

    m_read_characteristic = characteristic;

    const uint32_t err_code = sd_ble_gattc_read(conn_handle, characteristic->handle, 0);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to read attribute value, err 0x%04x"), err_code);
    }

    while (m_read_characteristic != NULL) {
        MICROPY_VM_HOOK_LOOP;
    }
}

STATIC void gattc_write(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo) {
    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);
    check_connected(conn_handle);

    ble_gattc_write_params_t write_params = {
        .write_op = characteristic->props.write_no_response ? BLE_GATT_OP_WRITE_CMD : BLE_GATT_OP_WRITE_REQ,
        .handle = characteristic->handle,
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
        mp_raise_OSError_msg_varg(translate("Failed to write attribute value, err 0x%04x"), err_code);
    }

}

STATIC void characteristic_on_ble_evt(ble_evt_t *ble_evt, void *param) {
    switch (ble_evt->header.evt_id) {

        // More events may be handled later, so keep this as a switch.

        case BLE_GATTC_EVT_READ_RSP: {
            ble_gattc_evt_read_rsp_t *response = &ble_evt->evt.gattc_evt.params.read_rsp;
            m_read_characteristic->value_data = mp_obj_new_bytearray(response->len, response->data);
            // Indicate to busy-wait loop that we've read the characteristic.
            m_read_characteristic = NULL;
            break;
        }

            // For debugging.
        default:
            // mp_printf(&mp_plat_print, "Unhandled characteristic event: 0x%04x\n", ble_evt->header.evt_id);
            break;
    }

}

void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self, bleio_uuid_obj_t *uuid, bleio_characteristic_properties_t props, mp_obj_list_t *descriptor_list) {
    self->service = mp_const_none;
    self->uuid = uuid;
    self->value_data = mp_const_none;
    self->props = props;
    self->descriptor_list = descriptor_list;
    self->handle = BLE_GATT_HANDLE_INVALID;

    ble_drv_add_event_handler(characteristic_on_ble_evt, self);
}

mp_obj_list_t *common_hal_bleio_characteristic_get_descriptor_list(bleio_characteristic_obj_t *self) {
    return self->descriptor_list;
}

mp_obj_t common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self) {
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

    return self->value_data;
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


bleio_uuid_obj_t *common_hal_bleio_characteristic_get_uuid(bleio_characteristic_obj_t *self) {
    return self->uuid;
}

bleio_characteristic_properties_t common_hal_bleio_characteristic_get_properties(bleio_characteristic_obj_t *self) {
    return self->props;
}

void common_hal_bleio_characteristic_set_cccd(bleio_characteristic_obj_t *self, bool notify, bool indicate) {
    if (self->cccd_handle == BLE_GATT_HANDLE_INVALID) {
        mp_raise_ValueError(translate("No CCCD for this Characteristic"));
    }

    if (common_hal_bleio_device_get_gatt_role(self->service->device) != GATT_ROLE_CLIENT) {
        mp_raise_ValueError(translate("Can't set CCCD for local Characteristic"));
    }

    uint16_t cccd_value =
        (notify ? BLE_GATT_HVX_NOTIFICATION : 0) |
        (indicate ? BLE_GATT_HVX_INDICATION : 0);

    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(self->service->device);
    check_connected(conn_handle);


    ble_gattc_write_params_t write_params = {
        .write_op = BLE_GATT_OP_WRITE_REQ,
        .handle = self->cccd_handle,
        .p_value = (uint8_t *) &cccd_value,
        .len = 2,
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
        mp_raise_OSError_msg_varg(translate("Failed to write CCCD, err 0x%04x"), err_code);
    }

}
