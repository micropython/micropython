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

#include "ble_drv.h"
#include "ble.h"
#include "py/runtime.h"
#include "common-hal/bleio/__init__.h"
#include "shared-bindings/bleio/Characteristic.h"
#include "shared-bindings/bleio/Descriptor.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/Adapter.h"

void common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, mp_obj_list_t *characteristic_list, bool is_secondary) {
    self->device = mp_const_none;
    self->handle = 0xFFFF;
    self->uuid = uuid;
    self->characteristic_list = characteristic_list;
    self->is_remote = false;
    self->is_secondary = is_secondary;

    for (size_t characteristic_idx = 0; characteristic_idx < characteristic_list->len; ++characteristic_idx) {
        bleio_characteristic_obj_t *characteristic =
            MP_OBJ_TO_PTR(characteristic_list->items[characteristic_idx]);
        characteristic->service = self;
    }
}

bleio_uuid_obj_t *common_hal_bleio_service_get_uuid(bleio_service_obj_t *self) {
    return self->uuid;
}

mp_obj_list_t *common_hal_bleio_service_get_characteristic_list(bleio_service_obj_t *self) {
    return self->characteristic_list;
}

bool common_hal_bleio_service_get_is_remote(bleio_service_obj_t *self) {
    return self->is_remote;
}

bool common_hal_bleio_service_get_is_secondary(bleio_service_obj_t *self) {
    return self->is_secondary;
}

// Call this after the Service has been added to the Peripheral.
void common_hal_bleio_service_add_all_characteristics(bleio_service_obj_t *self) {
    // Add all the characteristics.
    for (size_t characteristic_idx = 0; characteristic_idx < self->characteristic_list->len; ++characteristic_idx) {
        bleio_characteristic_obj_t *characteristic =
            MP_OBJ_TO_PTR(self->characteristic_list->items[characteristic_idx]);

        if (characteristic->handle != BLE_GATT_HANDLE_INVALID) {
            mp_raise_ValueError(translate("Characteristic already in use by another Service."));
        }

        ble_gatts_char_md_t char_md = {
            .char_props.broadcast      = (characteristic->props & CHAR_PROP_BROADCAST) ? 1 : 0,
            .char_props.read           = (characteristic->props & CHAR_PROP_READ) ? 1 : 0,
            .char_props.write_wo_resp  = (characteristic->props & CHAR_PROP_WRITE_NO_RESPONSE) ? 1 : 0,
            .char_props.write          = (characteristic->props & CHAR_PROP_WRITE) ? 1 : 0,
            .char_props.notify         = (characteristic->props & CHAR_PROP_NOTIFY) ? 1 : 0,
            .char_props.indicate       = (characteristic->props & CHAR_PROP_INDICATE) ? 1 : 0,
        };

        ble_gatts_attr_md_t cccd_md = {
            .vloc = BLE_GATTS_VLOC_STACK,
        };

        if (char_md.char_props.notify || char_md.char_props.indicate) {
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

            char_md.p_cccd_md = &cccd_md;
        }

        ble_uuid_t char_uuid;
        bleio_uuid_convert_to_nrf_ble_uuid(characteristic->uuid, &char_uuid);

        ble_gatts_attr_md_t char_attr_md = {
            .vloc = BLE_GATTS_VLOC_STACK,
            .vlen = !characteristic->fixed_length,
        };

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_attr_md.read_perm);
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_attr_md.write_perm);

        mp_buffer_info_t char_value_bufinfo;
        mp_get_buffer_raise(characteristic->value, &char_value_bufinfo, MP_BUFFER_READ);

        ble_gatts_attr_t char_attr = {
            .p_uuid = &char_uuid,
            .p_attr_md = &char_attr_md,
            .init_len = char_value_bufinfo.len,
            .p_value = char_value_bufinfo.buf,
            .init_offs = 0,
            .max_len = characteristic->max_length,
        };

        ble_gatts_char_handles_t char_handles;

        uint32_t err_code;
        err_code = sd_ble_gatts_characteristic_add(self->handle, &char_md, &char_attr, &char_handles);
        if (err_code != NRF_SUCCESS) {
            mp_raise_OSError_msg_varg(translate("Failed to add characteristic, err 0x%04x"), err_code);
        }

        characteristic->user_desc_handle = char_handles.user_desc_handle;
        characteristic->cccd_handle = char_handles.cccd_handle;
        characteristic->sccd_handle = char_handles.sccd_handle;
        characteristic->handle = char_handles.value_handle;

        // Add the descriptors for this characteristic.
        for (size_t descriptor_idx = 0; descriptor_idx < characteristic->descriptor_list->len; ++descriptor_idx) {
            bleio_descriptor_obj_t *descriptor =
                MP_OBJ_TO_PTR(characteristic->descriptor_list->items[descriptor_idx]);

            ble_uuid_t desc_uuid;
            bleio_uuid_convert_to_nrf_ble_uuid(descriptor->uuid, &desc_uuid);

            ble_gatts_attr_md_t desc_attr_md = {
                // Data passed is not in a permanent location and should be copied.
                .vloc = BLE_GATTS_VLOC_STACK,
                .vlen = !descriptor->fixed_length,
            };

            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&desc_attr_md.read_perm);
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&desc_attr_md.write_perm);

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

            err_code = sd_ble_gatts_descriptor_add(characteristic->handle, &desc_attr, &descriptor->handle);

        } // loop over descriptors

    } // loop over characteristics
}
