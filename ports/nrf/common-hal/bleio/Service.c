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
#include "py/nlr.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/Adapter.h"

void common_hal_bleio_service_add_characteristic(bleio_service_obj_t *self, bleio_characteristic_obj_t *characteristic) {
    ble_gatts_char_md_t char_md = {
        .char_props.broadcast      = characteristic->props.broadcast,
        .char_props.read           = characteristic->props.read,
        .char_props.write_wo_resp  = characteristic->props.write_wo_resp,
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

    ble_uuid_t uuid = {
        .type = BLE_UUID_TYPE_BLE,
        .uuid = characteristic->uuid->value[0] | (characteristic->uuid->value[1] << 8),
    };

    if (characteristic->uuid->type == UUID_TYPE_128BIT)
        uuid.type = characteristic->uuid->uuid_vs_idx;

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
        .max_len = (BLE_GATT_ATT_MTU_DEFAULT - 3),
    };

    ble_gatts_char_handles_t handles;

    uint32_t err_code;
    err_code = sd_ble_gatts_characteristic_add(self->handle, &char_md, &attr_char_value, &handles);
    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
            translate("Failed to add characteristic, status: 0x%08lX"), err_code));
    }

    characteristic->user_desc_handle = handles.user_desc_handle;
    characteristic->cccd_handle = handles.cccd_handle;
    characteristic->sccd_handle = handles.sccd_handle;
    characteristic->handle = handles.value_handle;
}
