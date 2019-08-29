/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#include "py/runtime.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"

static volatile bleio_descriptor_obj_t *m_read_descriptor;

void common_hal_bleio_descriptor_construct(bleio_descriptor_obj_t *self, bleio_characteristic_obj_t *characteristic, bleio_uuid_obj_t *uuid, bleio_attribute_security_mode_t read_perm, bleio_attribute_security_mode_t write_perm, mp_int_t max_length, bool fixed_length, mp_buffer_info_t *initial_value_bufinfo) {
    self->characteristic = characteristic;
    self->uuid = uuid;
    self->handle = BLE_GATT_HANDLE_INVALID;
    self->read_perm = read_perm;
    self->write_perm = write_perm;

    const mp_int_t max_length_max = fixed_length ? BLE_GATTS_FIX_ATTR_LEN_MAX : BLE_GATTS_VAR_ATTR_LEN_MAX;
    if (max_length < 0 || max_length > max_length_max) {
        mp_raise_ValueError_varg(translate("max_length must be 0-%d when fixed_length is %s"),
                                 max_length_max, fixed_length ? "True" : "False");
    }
    self->max_length = max_length;
    self->fixed_length = fixed_length;

    common_hal_bleio_descriptor_set_value(self, initial_value_bufinfo);
}

bleio_uuid_obj_t *common_hal_bleio_descriptor_get_uuid(bleio_descriptor_obj_t *self) {
    return self->uuid;
}

bleio_characteristic_obj_t *common_hal_bleio_descriptor_get_characteristic(bleio_descriptor_obj_t *self) {
    return self->characteristic;
}

STATIC void descriptor_on_gattc_read_rsp_evt(ble_evt_t *ble_evt, void *param) {
    switch (ble_evt->header.evt_id) {

        // More events may be handled later, so keep this as a switch.

        case BLE_GATTC_EVT_READ_RSP: {
            ble_gattc_evt_read_rsp_t *response = &ble_evt->evt.gattc_evt.params.read_rsp;
            if (m_read_descriptor) {
                m_read_descriptor->value = mp_obj_new_bytearray(response->len, response->data);
            }
            // Indicate to busy-wait loop that we've read the attribute value.
            m_read_descriptor = NULL;
            break;
        }

        default:
            // For debugging.
            // mp_printf(&mp_plat_print, "Unhandled descriptor event: 0x%04x\n", ble_evt->header.evt_id);
            break;
    }
}

STATIC void descriptor_gattc_read(bleio_descriptor_obj_t *descriptor) {
    const uint16_t conn_handle =
        common_hal_bleio_device_get_conn_handle(descriptor->characteristic->service->device);
    common_hal_bleio_check_connected(conn_handle);

    // Set to NULL in event loop after event.
    m_read_descriptor = descriptor;

    ble_drv_add_event_handler(descriptor_on_gattc_read_rsp_evt, descriptor);

    const uint32_t err_code = sd_ble_gattc_read(conn_handle, descriptor->handle, 0);
    if (err_code != NRF_SUCCESS) {
        mp_raise_OSError_msg_varg(translate("Failed to read attribute value, err 0x%04x"), err_code);
    }

    while (m_read_descriptor != NULL) {
        MICROPY_VM_HOOK_LOOP;
    }

    ble_drv_remove_event_handler(descriptor_on_gattc_read_rsp_evt, descriptor);
}

mp_obj_t common_hal_bleio_descriptor_get_value(bleio_descriptor_obj_t *self) {
    // Do GATT operations only if this descriptor has been registered
    if (self->handle != BLE_GATT_HANDLE_INVALID) {
        if (common_hal_bleio_service_get_is_remote(self->characteristic->service)) {
            descriptor_gattc_read(self);
        } else {
            self->value = common_hal_bleio_gatts_read(
                self->handle, common_hal_bleio_device_get_conn_handle(self->characteristic->service->device));
        }
    }

    return self->value;
}

void common_hal_bleio_descriptor_set_value(bleio_descriptor_obj_t *self, mp_buffer_info_t *bufinfo) {
    if (self->fixed_length && bufinfo->len != self->max_length) {
        mp_raise_ValueError(translate("Value length != required fixed length"));
    }
    if (bufinfo->len > self->max_length) {
        mp_raise_ValueError(translate("Value length > max_length"));
    }

    self->value = mp_obj_new_bytes(bufinfo->buf, bufinfo->len);

    // Do GATT operations only if this descriptor has been registered.
    if (self->handle != BLE_GATT_HANDLE_INVALID) {
        uint16_t conn_handle = common_hal_bleio_device_get_conn_handle(self->characteristic->service->device);
        if (common_hal_bleio_service_get_is_remote(self->characteristic->service)) {
            // false means WRITE_REQ, not write-no-response
            common_hal_bleio_gattc_write(self->handle, conn_handle, bufinfo, false);
        } else {
            common_hal_bleio_gatts_write(self->handle, conn_handle, bufinfo);
        }
    }

}
