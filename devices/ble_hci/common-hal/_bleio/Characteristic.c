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
#include "common-hal/_bleio/att.h"

#define CCCD_NOTIFY   0x1
#define CCCD_INDICATE 0x2


void common_hal_bleio_characteristic_construct(bleio_characteristic_obj_t *self, bleio_service_obj_t *service, uint16_t handle, bleio_uuid_obj_t *uuid, bleio_characteristic_properties_t props, bleio_attribute_security_mode_t read_perm, bleio_attribute_security_mode_t write_perm, mp_int_t max_length, bool fixed_length, mp_buffer_info_t *initial_value_bufinfo) {
    self->service = service;
    self->uuid = uuid;
    self->decl_handle = BLE_GATT_HANDLE_INVALID;
    self->handle = BLE_GATT_HANDLE_INVALID;
    self->props = props;
    self->read_perm = read_perm;
    self->write_perm = write_perm;
    self->descriptor_list = NULL;

    const mp_int_t max_length_max = 512;
    if (max_length < 0 || max_length > max_length_max) {
        mp_raise_ValueError(translate("max_length must be <= 512"));
    }
    self->max_length = max_length;
    self->fixed_length = fixed_length;

    if (service->is_remote) {
        self->handle = handle;
    } else {
        common_hal_bleio_service_add_characteristic(self->service, self, initial_value_bufinfo);
    }

    if (initial_value_bufinfo != NULL) {
        common_hal_bleio_characteristic_set_value(self, initial_value_bufinfo);
    }
}

bleio_descriptor_obj_t *common_hal_bleio_characteristic_get_descriptor_list(bleio_characteristic_obj_t *self) {
    return self->descriptor_list;
}

bleio_service_obj_t *common_hal_bleio_characteristic_get_service(bleio_characteristic_obj_t *self) {
    return self->service;
}

size_t common_hal_bleio_characteristic_get_value(bleio_characteristic_obj_t *self, uint8_t* buf, size_t len) {
    // Do GATT operations only if this characteristic has been added to a registered service.
    if (self->handle != BLE_GATT_HANDLE_INVALID) {
        uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
        if (common_hal_bleio_service_get_is_remote(self->service)) {
            // self->value is set by evt handler.
            return common_hal_bleio_gattc_read(self->handle, conn_handle, buf, len);
        } else {
            // conn_handle is ignored for non-system attributes.
            return common_hal_bleio_gatts_read(self->handle, conn_handle, buf, len);
        }
    }

    return 0;
}

void common_hal_bleio_characteristic_set_value(bleio_characteristic_obj_t *self, mp_buffer_info_t *bufinfo) {
    if (self->fixed_length && bufinfo->len != self->max_length) {
        mp_raise_ValueError(translate("Value length != required fixed length"));
    }
    if (bufinfo->len > self->max_length) {
        mp_raise_ValueError(translate("Value length > max_length"));
    }

    // Do GATT operations only if this characteristic has been added to a registered service.
    if (self->handle != BLE_GATT_HANDLE_INVALID) {

        if (common_hal_bleio_service_get_is_remote(self->service)) {
            uint16_t conn_handle = bleio_connection_get_conn_handle(self->service->connection);
            // Last argument is true if write-no-reponse desired.
            common_hal_bleio_gattc_write(self->handle, conn_handle, bufinfo,
                                         (self->props & CHAR_PROP_WRITE_NO_RESPONSE));
        } else {
            // Always write the value locally even if no connections are active.
            // conn_handle is ignored for non-system attributes, so we use BLE_CONN_HANDLE_INVALID.
            common_hal_bleio_gatts_write(self->handle, BLE_CONN_HANDLE_INVALID, bufinfo);
            // Notify or indicate all active connections.
            uint16_t cccd = 0;

            const bool notify = self->props & CHAR_PROP_NOTIFY;
            const bool indicate = self->props & CHAR_PROP_INDICATE;
            // Read the CCCD value, if there is one.
            if ((notify | indicate) && self->cccd_handle != BLE_GATT_HANDLE_INVALID) {
                common_hal_bleio_gatts_read(self->cccd_handle, BLE_CONN_HANDLE_INVALID,
                                            (uint8_t *) &cccd, sizeof(cccd));
            }

            // It's possible that both notify and indicate are set.
            if (notify && (cccd & CCCD_NOTIFY)) {
                att_notify(self->handle, bufinfo->buf, MIN(bufinfo->len, self->max_length));
            }
            if (indicate && (cccd & CCCD_INDICATE)) {
                att_indicate(self->handle, bufinfo->buf, MIN(bufinfo->len, self->max_length));

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
    if (self->handle != common_hal_bleio_adapter_obj.last_added_characteristic_handle) {
        mp_raise_bleio_BluetoothError(
            translate("Descriptor can only be added to most recently added characteristic"));
    }

    descriptor->handle = bleio_adapter_add_attribute(&common_hal_bleio_adapter_obj, MP_OBJ_TO_PTR(descriptor));
    // Include this descriptor in the service handle's range.
    self->service->end_handle = descriptor->handle;

    // Link together all the descriptors for this characteristic.
    descriptor->next = self->descriptor_list;
    self->descriptor_list = descriptor;
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
        (notify ? CCCD_NOTIFY : 0) |
        (indicate ? CCCD_INDICATE : 0);

    (void) cccd_value;
    //FIX call att_something to set remote CCCD
    // ble_gattc_write_params_t write_params = {
    //     .write_op = BLE_GATT_OP_WRITE_REQ,
    //     .handle = self->cccd_handle,
    //     .p_value = (uint8_t *) &cccd_value,
    //     .len = 2,
    // };

    // while (1) {
    //     uint32_t err_code = sd_ble_gattc_write(conn_handle, &write_params);
    //     if (err_code == NRF_SUCCESS) {
    //         break;
    //     }

    //     // Write with response will return NRF_ERROR_BUSY if the response has not been received.
    //     // Write without reponse will return NRF_ERROR_RESOURCES if too many writes are pending.
    //     if (err_code == NRF_ERROR_BUSY || err_code == NRF_ERROR_RESOURCES) {
    //         // We could wait for an event indicating the write is complete, but just retrying is easier.
    //         RUN_BACKGROUND_TASKS;
    //         continue;
    //     }

    //     // Some real error occurred.
    //     check_nrf_error(err_code);
    // }

}
