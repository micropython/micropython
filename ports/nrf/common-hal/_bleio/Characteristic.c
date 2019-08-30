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

#include "py/runtime.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"

static volatile bleio_characteristic_obj_t *m_read_characteristic;

STATIC uint16_t characteristic_get_cccd(uint16_t cccd_handle, uint16_t conn_handle) {
    uint16_t cccd;
    ble_gatts_value_t value = {
        .p_value = (uint8_t*) &cccd,
        .len = 2,
    };

    const uint32_t err_code = sd_ble_gatts_value_get(conn_handle, cccd_handle, &value);

    if (err_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING) {
        // CCCD is not set, so say that neither Notify nor Indicate is enabled.
        cccd = 0;
    } else if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to read CCCD value, err 0x%04x"), err_code);
    }

    return cccd;
}

STATIC void characteristic_on_gattc_read_rsp_evt(ble_evt_t *ble_evt, void *param) {
    switch (ble_evt->header.evt_id) {

        // More events may be handled later, so keep this as a switch.

        case BLE_GATTC_EVT_READ_RSP: {
            ble_gattc_evt_read_rsp_t *response = &ble_evt->evt.gattc_evt.params.read_rsp;
            if (m_read_characteristic) {
                m_read_characteristic->value = mp_obj_new_bytearray(response->len, response->data);
            }
            // Indicate to busy-wait loop that we've read the attribute value.
            m_read_characteristic = NULL;
            break;
        }

        default:
            // For debugging.
            // mp_printf(&mp_plat_print, "Unhandled characteristic event: 0x%04x\n", ble_evt->header.evt_id);
            break;
    }
}

STATIC void characteristic_gatts_notify_indicate(uint16_t handle, uint16_t conn_handle, mp_buffer_info_t *bufinfo, uint16_t hvx_type) {
    uint16_t hvx_len = bufinfo->len;

    ble_gatts_hvx_params_t hvx_params = {
        .handle = handle,
        .type = hvx_type,
        .offset = 0,
        .p_len = &hvx_len,
        .p_data = bufinfo->buf,
    };

    while (1) {
        const uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
        if (err_code == NRF_SUCCESS) {
            break;
        }
        // TX buffer is full
        // We could wait for an event indicating the write is complete, but just retrying is easier.
        if (err_code == NRF_ERROR_RESOURCES) {
            RUN_BACKGROUND_TASKS;
            continue;
        }

        // Some real error has occurred.
        mp_raise_OSError_msg_varg(translate("Failed to notify or indicate attribute value, err 0x%04x"), err_code);
    }
}

STATIC void characteristic_gattc_read(bleio_characteristic_obj_t *characteristic) {
    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(characteristic->service->device);
    common_hal_bleio_check_connected(conn_handle);

    // Set to NULL in event loop after event.
    m_read_characteristic = characteristic;

    ble_drv_add_event_handler(characteristic_on_gattc_read_rsp_evt, characteristic);

    const uint32_t err_code = sd_ble_gattc_read(conn_handle, characteristic->handle, 0);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to read attribute value, err 0x%04x"), err_code);
    }

    while (m_read_characteristic != NULL) {
        RUN_BACKGROUND_TASKS;
    }

    ble_drv_remove_event_handler(characteristic_on_gattc_read_rsp_evt, characteristic);
}

void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self, bleio_service_obj_t *service, bleio_uuid_obj_t *uuid, bleio_characteristic_properties_t props, bleio_attribute_security_mode_t read_perm, bleio_attribute_security_mode_t write_perm, mp_int_t max_length, bool fixed_length, mp_buffer_info_t *initial_value_bufinfo) {
    self->service = service;
    self->uuid = uuid;
    self->handle = BLE_GATT_HANDLE_INVALID;
    self->props = props;
    self->read_perm = read_perm;
    self->write_perm = write_perm;
    self->descriptor_list = mp_obj_new_list(0, NULL);

    const mp_int_t max_length_max = fixed_length ? BLE_GATTS_FIX_ATTR_LEN_MAX : BLE_GATTS_VAR_ATTR_LEN_MAX;
    if (max_length < 0 || max_length > max_length_max) {
        mp_raise_ValueError_varg(translate("max_length must be 0-%d when fixed_length is %s"),
                                 max_length_max, fixed_length ? "True" : "False");
    }
    self->max_length = max_length;
    self->fixed_length = fixed_length;

    common_hal_bleio_characteristic_set_value(self, initial_value_bufinfo);
}

mp_obj_list_t *common_hal_bleio_characteristic_get_descriptor_list(bleio_characteristic_obj_t *self) {
    return self->descriptor_list;
}

bleio_service_obj_t *common_hal_bleio_characteristic_get_service(bleio_characteristic_obj_t *self) {
    return self->service;
}

mp_obj_t common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self) {
    // Do GATT operations only if this characteristic has been added to a registered service.
    if (self->handle != BLE_GATT_HANDLE_INVALID) {
        uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(self->service->device);
        if (common_hal_bleio_service_get_is_remote(self->service)) {
            // self->value is set by evt handler.
            characteristic_gattc_read(self);
        } else {
            self->value = common_hal_bleio_gatts_read(self->handle, conn_handle);
        }
    }

    return self->value;
}

void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    if (self->fixed_length && bufinfo->len != self->max_length) {
        mp_raise_ValueError(translate("Value length != required fixed length"));
    }
    if (bufinfo->len > self->max_length) {
        mp_raise_ValueError(translate("Value length > max_length"));
    }

    self->value = mp_obj_new_bytes(bufinfo->buf, bufinfo->len);

    // Do GATT operations only if this characteristic has been added to a registered service.
    if (self->handle != BLE_GATT_HANDLE_INVALID) {
        uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(self->service->device);

        if (common_hal_bleio_service_get_is_remote(self->service)) {
            // Last argument is true if write-no-reponse desired.
            common_hal_bleio_gattc_write(self->handle, conn_handle, bufinfo,
                                         (self->props & CHAR_PROP_WRITE_NO_RESPONSE));
        } else {

            bool sent = false;
            uint16_t cccd = 0;

            const bool notify = self->props & CHAR_PROP_NOTIFY;
            const bool indicate = self->props & CHAR_PROP_INDICATE;
            if (notify | indicate) {
                cccd = characteristic_get_cccd(self->cccd_handle, conn_handle);
            }

            // It's possible that both notify and indicate are set.
            if (notify && (cccd & BLE_GATT_HVX_NOTIFICATION)) {
                characteristic_gatts_notify_indicate(self->handle, conn_handle, bufinfo, BLE_GATT_HVX_NOTIFICATION);
                sent = true;
            }
            if (indicate && (cccd & BLE_GATT_HVX_INDICATION)) {
                characteristic_gatts_notify_indicate(self->handle, conn_handle, bufinfo, BLE_GATT_HVX_INDICATION);
                sent = true;
            }

            if (!sent) {
                common_hal_bleio_gatts_write(self->handle, conn_handle, bufinfo);
            }
        }
    }
}

bleio_uuid_obj_t *common_hal_bleio_characteristic_get_uuid(bleio_characteristic_obj_t *self) {
    return self->uuid;
}

bleio_characteristic_properties_t common_hal_bleio_characteristic_get_properties(bleio_characteristic_obj_t *self) {
    return self->props;
}

void common_hal_bleio_characteristic_add_descriptor(bleio_characteristic_obj_t *self, bleio_descriptor_obj_t *descriptor) {
    ble_uuid_t desc_uuid;
    bleio_uuid_convert_to_nrf_ble_uuid(descriptor->uuid, &desc_uuid);

    ble_gatts_attr_md_t desc_attr_md = {
        // Data passed is not in a permanent location and should be copied.
        .vloc = BLE_GATTS_VLOC_STACK,
        .vlen = !descriptor->fixed_length,
    };

    bleio_attribute_gatts_set_security_mode(&desc_attr_md.read_perm, descriptor->read_perm);
    bleio_attribute_gatts_set_security_mode(&desc_attr_md.write_perm, descriptor->write_perm);

    mp_buffer_info_t desc_value_bufinfo;
    mp_get_buffer_raise(descriptor->value, &desc_value_bufinfo, MP_BUFFER_READ);

    ble_gatts_attr_t desc_attr = {
        .p_uuid = &desc_uuid,
        .p_attr_md = &desc_attr_md,
        .init_len = desc_value_bufinfo.len,
        .p_value = desc_value_bufinfo.buf,
        .init_offs = 0,
        .max_len = descriptor->max_length,
    };

    uint32_t err_code = sd_ble_gatts_descriptor_add(self->handle, &desc_attr, &descriptor->handle);

    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to add descriptor, err 0x%04x"), err_code);
    }

    mp_obj_list_append(self->descriptor_list, MP_OBJ_FROM_PTR(descriptor));
}

void common_hal_bleio_characteristic_set_cccd(bleio_characteristic_obj_t *self, bool notify, bool indicate) {
    if (self->cccd_handle == BLE_GATT_HANDLE_INVALID) {
        mp_raise_ValueError(translate("No CCCD for this Characteristic"));
    }

    if (!common_hal_bleio_service_get_is_remote(self->service)) {
        mp_raise_ValueError(translate("Can't set CCCD on local Characteristic"));
    }

    const uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(self->service->device);
    common_hal_bleio_check_connected(conn_handle);

    uint16_t cccd_value =
        (notify ? BLE_GATT_HVX_NOTIFICATION : 0) |
        (indicate ? BLE_GATT_HVX_INDICATION : 0);

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
            RUN_BACKGROUND_TASKS;
            continue;
        }

        // Some real error occurred.
        mp_raise_OSError_msg_varg(translate("Failed to write CCCD, err 0x%04x"), err_code);
    }

}
