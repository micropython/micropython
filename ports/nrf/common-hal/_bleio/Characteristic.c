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

#include "common-hal/_bleio/Adapter.h"
#include "common-hal/_bleio/bonding.h"

STATIC uint16_t characteristic_get_cccd(uint16_t cccd_handle, uint16_t conn_handle) {
    uint16_t cccd;
    ble_gatts_value_t value = {
        .p_value = (uint8_t *)&cccd,
        .len = 2,
    };

    const uint32_t err_code = sd_ble_gatts_value_get(conn_handle, cccd_handle, &value);

    if (err_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING) {
        // CCCD is not set, so say that neither Notify nor Indicate is enabled.
        cccd = 0;
    } else {
        check_nrf_error(err_code);
    }

    return cccd;
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
        check_nrf_error(err_code);
    }
}

void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self, bleio_service_obj_t *service,
    uint16_t handle, bleio_uuid_obj_t *uuid, bleio_characteristic_properties_t props,
    bleio_attribute_security_mode_t read_perm, bleio_attribute_security_mode_t write_perm,
    mp_int_t max_length, bool fixed_length, mp_buffer_info_t *initial_value_bufinfo,
    const char *user_description) {
    self->service = service;
    self->uuid = uuid;
    self->handle = BLE_GATT_HANDLE_INVALID;
    self->props = props;
    self->read_perm = read_perm;
    self->write_perm = write_perm;
    self->initial_value_len = 0;
    self->initial_value = NULL;
    if (initial_value_bufinfo != NULL) {
        // Copy the initial value if it's on the heap. Otherwise it's internal and we may not be able
        // to allocate.
        self->initial_value_len = initial_value_bufinfo->len;
        if (gc_alloc_possible()) {
            if (gc_nbytes(initial_value_bufinfo->buf) > 0) {
                uint8_t *initial_value = m_malloc(self->initial_value_len, false);
                memcpy(initial_value, initial_value_bufinfo->buf, self->initial_value_len);
                self->initial_value = initial_value;
            } else {
                self->initial_value = initial_value_bufinfo->buf;
            }
            self->descriptor_list = mp_obj_new_list(0, NULL);
        } else {
            self->initial_value = initial_value_bufinfo->buf;
            self->descriptor_list = NULL;
        }
    }

    const mp_int_t max_length_max = fixed_length ? BLE_GATTS_FIX_ATTR_LEN_MAX : BLE_GATTS_VAR_ATTR_LEN_MAX;
    if (max_length < 0 || max_length > max_length_max) {
        mp_raise_ValueError_varg(translate("max_length must be 0-%d when fixed_length is %s"),
            max_length_max, fixed_length ? "True" : "False");
    }
    self->max_length = max_length;
    self->fixed_length = fixed_length;

    if (service->is_remote) {
        self->handle = handle;
    } else {
        common_hal_bleio_service_add_characteristic(self->service, self, initial_value_bufinfo, user_description);
    }
}

mp_obj_tuple_t *common_hal_bleio_characteristic_get_descriptors(bleio_characteristic_obj_t *self) {
    if (self->descriptor_list == NULL) {
        return mp_const_empty_tuple;
    }
    return mp_obj_new_tuple(self->descriptor_list->len, self->descriptor_list->items);
}

bleio_service_obj_t *common_hal_bleio_characteristic_get_service(bleio_characteristic_obj_t *self) {
    return self->service;
}

size_t common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self, uint8_t *buf, size_t len) {
    // Do GATT operations only if this characteristic has been added to a registered service.
    if (self->handle != BLE_GATT_HANDLE_INVALID) {
        uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
        if (common_hal_bleio_service_get_is_remote(self->service)) {
            return common_hal_bleio_gattc_read(self->handle, conn_handle, buf, len);
        } else {
            // conn_handle is ignored for non-system attributes.
            return common_hal_bleio_gatts_read(self->handle, conn_handle, buf, len);
        }
    }

    return 0;
}

size_t common_hal_bleio_characteristic_get_max_length(bleio_characteristic_obj_t *self) {
    return self->max_length;
}

void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    // Do GATT operations only if this characteristic has been added to a registered service.
    if (self->handle != BLE_GATT_HANDLE_INVALID) {

        if (common_hal_bleio_service_get_is_remote(self->service)) {
            uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
            // Last argument is true if write-no-reponse desired.
            common_hal_bleio_gattc_write(self->handle, conn_handle, bufinfo,
                (self->props & CHAR_PROP_WRITE_NO_RESPONSE));
        } else {
            // Validate data length for local characteristics only.
            if (self->fixed_length && bufinfo->len != self->max_length) {
                mp_raise_ValueError(translate("Value length != required fixed length"));
            }
            if (bufinfo->len > self->max_length) {
                mp_raise_ValueError(translate("Value length > max_length"));
            }

            // Always write the value locally even if no connections are active.
            // conn_handle is ignored for non-system attributes, so we use BLE_CONN_HANDLE_INVALID.
            common_hal_bleio_gatts_write(self->handle, BLE_CONN_HANDLE_INVALID, bufinfo);
            // Check to see if we need to notify or indicate any active connections.
            for (size_t i = 0; i < BLEIO_TOTAL_CONNECTION_COUNT; i++) {
                bleio_connection_internal_t *connection = &bleio_connections[i];
                uint16_t conn_handle = connection->conn_handle;
                if (connection->conn_handle == BLE_CONN_HANDLE_INVALID) {
                    continue;
                }

                uint16_t cccd = 0;

                const bool notify = self->props & CHAR_PROP_NOTIFY;
                const bool indicate = self->props & CHAR_PROP_INDICATE;
                if (notify | indicate) {
                    cccd = characteristic_get_cccd(self->cccd_handle, conn_handle);
                }

                // It's possible that both notify and indicate are set.
                if (notify && (cccd & BLE_GATT_HVX_NOTIFICATION)) {
                    characteristic_gatts_notify_indicate(self->handle, conn_handle, bufinfo, BLE_GATT_HVX_NOTIFICATION);
                }
                if (indicate && (cccd & BLE_GATT_HVX_INDICATION)) {
                    characteristic_gatts_notify_indicate(self->handle, conn_handle, bufinfo, BLE_GATT_HVX_INDICATION);
                }
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
    if (self->descriptor_list == NULL) {
        // This should only happen from internal use so we just fail silently instead of raising an
        // exception.
        return;
    }
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
    mp_get_buffer_raise(descriptor->initial_value, &desc_value_bufinfo, MP_BUFFER_READ);

    ble_gatts_attr_t desc_attr = {
        .p_uuid = &desc_uuid,
        .p_attr_md = &desc_attr_md,
        .init_len = desc_value_bufinfo.len,
        .p_value = desc_value_bufinfo.buf,
        .init_offs = 0,
        .max_len = descriptor->max_length,
    };

    check_nrf_error(sd_ble_gatts_descriptor_add(self->handle, &desc_attr, &descriptor->handle));

    mp_obj_list_append(MP_OBJ_FROM_PTR(self->descriptor_list),
        MP_OBJ_FROM_PTR(descriptor));
}

void common_hal_bleio_characteristic_set_cccd(bleio_characteristic_obj_t *self, bool notify, bool indicate) {
    if (self->cccd_handle == BLE_GATT_HANDLE_INVALID) {
        mp_raise_bleio_BluetoothError(translate("No CCCD for this Characteristic"));
    }

    if (!common_hal_bleio_service_get_is_remote(self->service)) {
        mp_raise_bleio_RoleError(translate("Can't set CCCD on local Characteristic"));
    }

    const uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
    common_hal_bleio_check_connected(conn_handle);

    uint16_t cccd_value =
        (notify ? BLE_GATT_HVX_NOTIFICATION : 0) |
        (indicate ? BLE_GATT_HVX_INDICATION : 0);

    ble_gattc_write_params_t write_params = {
        .write_op = BLE_GATT_OP_WRITE_REQ,
        .handle = self->cccd_handle,
        .p_value = (uint8_t *)&cccd_value,
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
        check_nrf_error(err_code);
    }

}
