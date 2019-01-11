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

#include "ble_drv.h"
#include "ble.h"
#include "py/runtime.h"
#include "common-hal/bleio/__init__.h"
#include "common-hal/bleio/Characteristic.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/Adapter.h"

void common_hal_bleio_service_construct(bleio_service_obj_t *self) {
}

// Call this after the Service has been added to the Peripheral.
void common_hal_bleio_service_add_all_characteristics(bleio_service_obj_t *self) {
    // Add all the characteristics.
    const mp_obj_list_t *char_list = MP_OBJ_TO_PTR(self->char_list);
    for (size_t char_idx = 0; char_idx < char_list->len; ++char_idx) {
        bleio_characteristic_obj_t *characteristic = char_list->items[char_idx];

        ble_gatts_char_md_t char_md = {
            .char_props.broadcast      = characteristic->props.broadcast,
            .char_props.read           = characteristic->props.read,
            .char_props.write_wo_resp  = characteristic->props.write_no_response,
            .char_props.write          = characteristic->props.write,
            .char_props.notify         = characteristic->props.notify,
            .char_props.indicate       = characteristic->props.indicate,
        };

        ble_gatts_attr_md_t cccd_md = {
            .vloc = BLE_GATTS_VLOC_STACK,
        };

        if (char_md.char_props.notify || char_md.char_props.indicate) {
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
            BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

            char_md.p_cccd_md = &cccd_md;
        }

        ble_uuid_t uuid;
        bleio_uuid_convert_to_nrf_ble_uuid(characteristic->uuid, &uuid);

        ble_gatts_attr_md_t attr_md = {
            .vloc = BLE_GATTS_VLOC_STACK,
            .vlen = 1,
        };

        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

        ble_gatts_attr_t attr_char_value = {
            .p_uuid = &uuid,
            .p_attr_md = &attr_md,
            .init_len = sizeof(uint8_t),
            .max_len = GATT_MAX_DATA_LENGTH,
        };

        ble_gatts_char_handles_t handles;

        uint32_t err_code;
        err_code = sd_ble_gatts_characteristic_add(self->handle, &char_md, &attr_char_value, &handles);
        if (err_code != NRF_SUCCESS) {
            mp_raise_OSError_msg_varg(translate("Failed to add characteristic, err 0x%04x"), err_code);
        }

        if (characteristic->handle != BLE_GATT_HANDLE_INVALID) {
            mp_raise_ValueError(translate("Characteristic already in use by another Service."));
        }

        characteristic->user_desc_handle = handles.user_desc_handle;
        characteristic->cccd_handle = handles.cccd_handle;
        characteristic->sccd_handle = handles.sccd_handle;
        characteristic->handle = handles.value_handle;
    }
}
